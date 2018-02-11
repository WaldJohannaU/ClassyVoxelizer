/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "ClassyVoxelizer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "tinyply.h"
#include "MultiClassVoxelGrid.h"
#include "MultiClassVoxelizer.h"
#include "ColoredVoxelizer.h"
#include "ColoredVoxelGrid.h"

ClassyVoxelizer::ClassyVoxelizer(const float voxel_size): voxel_size_(voxel_size) {
    
}

void ClassyVoxelizer::Process(const VoxelType voxel_type,
                              const std::string& input_file,
                              const std::string& output_file,
                              const std::string& mesh_output) {
    vertices_.clear();
    faces_.clear();
    vertex_classes_.clear();
    vertex_labels_.clear();
    colormap_.clear();
    colors_.clear();
    
    if (voxel_type == VoxelType::label) {
        ReadPly(input_file);
        // if the property label for the class was not found the classes are computed from, the color
        if (vertex_labels_.empty())
            ComputeClassFromColor(vertex_classes_);
        else
            ComputeColorFromLabel(vertex_labels_, num_labels_);
    } else if (voxel_type == VoxelType::color) {
        ReadPly(input_file);
    }
    GetVoxelSpaceDimensions(voxel_size_);
    std::cout << "Voxelizing at " << voxel_size_ << "m resolution: " << std::flush;
    
    if (voxel_type == VoxelType::label) {
        MultiClassVoxelizer voxelizer;
        MultiClassVoxelGrid voxelgrid(min_, max_, voxel_size_);
        voxelgrid.class_color_mapping = colormap_;
        voxelizer.Voxelize(voxelgrid, vertices_, faces_, vertex_labels_.empty() ? vertex_classes_ : vertex_labels_);
        voxelgrid.SaveAsPLY(output_file);
        voxelgrid.SaveAsPLYMesh(mesh_output);
    } else if (voxel_type == VoxelType::color) {
        ColoredVoxelizer voxelizer;
        ColoredVoxelGrid voxelgrid(min_, max_, voxel_size_);
        voxelizer.Voxelize(voxelgrid, vertices_, faces_, colors_);
        voxelgrid.SaveAsPLY(output_file);
        voxelgrid.SaveAsPLYMesh(mesh_output);
    }
}

int ClassyVoxelizer::ReadPly(const std::string& filepath) {
    std::ifstream ss(filepath, std::ios::binary);
    tinyply::PlyFile input_file(ss);
    std::vector<float> raw_vertices;
    std::vector<uint8_t> raw_colors;
    std::vector<uint16_t> raw_labels;
    
    const uint32_t num_vertices = input_file.request_properties_from_element("vertex", { "x", "y", "z" }, raw_vertices);
    input_file.request_properties_from_element("vertex", { "red", "green", "blue" }, raw_colors);
    input_file.request_properties_from_element("vertex", { "label" }, raw_labels);
    input_file.request_properties_from_element("face", { "vertex_indices" }, faces_, 3);

    input_file.read(ss);
    
    vertices_.resize(num_vertices);
    colors_.resize(num_vertices);
    if (raw_labels.size() == num_vertices)
        vertex_labels_.resize(num_vertices);
    
    int raw_vertices_i = 0;
    int raw_colors_i = 0;
    for (int i = 0; i < num_vertices; i++) {
        vertices_[i][0] = raw_vertices[raw_vertices_i++];
        vertices_[i][1] = raw_vertices[raw_vertices_i++];
        vertices_[i][2] = raw_vertices[raw_vertices_i++];
        colors_[i][0] = raw_colors[raw_colors_i++];
        colors_[i][1] = raw_colors[raw_colors_i++];
        colors_[i][2] = raw_colors[raw_colors_i++];
        if (raw_labels.size() == num_vertices)
            vertex_labels_[i] = raw_labels[i];
    }
    return num_vertices;
}

// create map with unique colors in the scene
bool ClassyVoxelizer::CreateColorMap(const std::vector<Eigen::Vector3i>& colors,
                                     std::vector<Eigen::Vector3i>& colormap) {
    for (const auto& color: colors) {
        if (std::find(colormap.begin(), colormap.end(), color) == colormap.end())
            colormap.push_back(color);
    }
    if (colormap.size() > 255) {
        std::cerr << "Error: ClassyVoxelizer only supports up to 255 classes." << std::endl;
        return false;
    }
    return true;
}

bool ClassyVoxelizer::ComputeColorFromLabel(std::vector<uint16_t>& labels,
                                            const int num_labels) {
    colormap_.resize(num_labels);
    std::fill(colormap_.begin(), colormap_.end(), Eigen::Vector3i(250, 0, 0));
    
    std::vector<uint16_t> labelmap;
    for (int i = 0; i < colors_.size(); i++) {
        if (std::find(labelmap.begin(), labelmap.end(), labels[i]) == labelmap.end()) {
            // new label found
            const int label = labels[i];
            labelmap.push_back(label);
            if (label > colormap_.size()) {
                std::cout << "skip label in ply (index above threshold)." << std::endl;
                continue;
            }
            colormap_[label] = colors_[i];
        }
    }
    // map of labels
    if (labelmap.size() > 255) {
        std::cerr << "Error: ClassyVoxelizer only supports up to 255 classes." << std::endl;
        return false;
    }
    return true;
}

bool ClassyVoxelizer::ComputeClassFromColor(std::vector<uint16_t>& classes) {
    classes.resize(colors_.size());
    CreateColorMap(colors_, colormap_);
    
    int vertex_class_i = 0;
    for (const auto& color: colors_) {
        int class_i;
        for (class_i = 0; class_i < colormap_.size(); class_i++) {
            if (colormap_[class_i] == color) {
                break;
            }
        }
        classes[vertex_class_i++] = class_i+1;
    }
    return true;
}

void ClassyVoxelizer::GetVoxelSpaceDimensions(const double voxel_size) {
    Eigen::Vector3f centroid(0,0,0);
    
    for (const Eigen::Vector3f& vertex: vertices_)
        centroid += vertex;
    centroid /= vertices_.size();
    
    max_ = centroid;
    min_ = centroid;
    
    for (const Eigen::Vector3f& vertex: vertices_) {
        if ((vertex[0] - centroid[0]) > (max_[0] - centroid[0]))
            max_[0] = vertex[0];
        
        if ((vertex[1] - centroid[1]) > (max_[1] - centroid[1]))
            max_[1] = vertex[1];
        
        if ((vertex[2] - centroid[2]) > (max_[2] - centroid[2]))
            max_[2] = vertex[2];
        
        if ((vertex[0] - centroid[0]) < (min_[0] - centroid[0]))
            min_[0] = vertex[0];
        
        if ((vertex[1] - centroid[1]) < (min_[1] - centroid[1]))
            min_[1] = vertex[1];
        
        if ((vertex[2] - centroid[2]) < (min_[2] - centroid[2]))
            min_[2] = vertex[2];
    }
    
    max_ += Eigen::Vector3f(voxel_size_, voxel_size_, voxel_size_);
    min_ -= Eigen::Vector3f(voxel_size_, voxel_size_, voxel_size_);
}
