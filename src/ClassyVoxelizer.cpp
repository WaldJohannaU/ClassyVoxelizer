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

void ClassyVoxelizer::Process(const VoxelType voxel_type,
                              const std::string& input_file,
                              const std::string& output_file,
                              const float voxel_size,
                              const std::string& mesh_output) {
    vertices.clear();
    faces.clear();
    vertex_classes.clear();
    vertex_labels.clear();
    colormap.clear();
    colors.clear();
    
    if (voxel_type == VoxelType::label) {
        ReadPly(input_file);
        // if the property label for the class was not found the classes are computed from, the color
        if (vertex_labels.empty())
            ComputeClassFromColor(vertex_classes);
        else
            ComputeColorFromLabel(vertex_labels, num_labels);
    } else if (voxel_type == VoxelType::color) {
        ReadPly(input_file);
    }
    GetVoxelSpaceDimensions(voxel_size);
    std::cout << "Voxelizing at " << voxel_size << "m resolution: " << std::flush;
    
    if (voxel_type == VoxelType::label) {
        MultiClassVoxelizer voxelizer;
        MultiClassVoxelGrid voxelgrid(min, max, voxel_size);
        voxelgrid.class_color_mapping = colormap;
        voxelizer.Voxelize(voxelgrid, vertices, faces, vertex_labels.empty() ? vertex_classes : vertex_labels);
        voxelgrid.SaveAsPLY(output_file);
    } else if (voxel_type == VoxelType::color) {
        ColoredVoxelizer voxelizer;
        ColoredVoxelGrid voxelgrid(min, max, voxel_size);
        voxelizer.Voxelize(voxelgrid, vertices, faces, colors);
        voxelgrid.SaveAsPLY(output_file);
    }
    // if (mesh_output != "")
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
    input_file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
    
    input_file.read(ss);
    
    vertices.resize(num_vertices);
    colors.resize(num_vertices);
    if (raw_labels.size() == num_vertices)
        vertex_labels.resize(num_vertices);
    
    int raw_vertices_i = 0;
    int raw_colors_i = 0;
    for (int i = 0; i < num_vertices; i++) {
        vertices[i][0] = raw_vertices[raw_vertices_i++];
        vertices[i][1] = raw_vertices[raw_vertices_i++];
        vertices[i][2] = raw_vertices[raw_vertices_i++];
        colors[i][0] = raw_colors[raw_colors_i++];
        colors[i][1] = raw_colors[raw_colors_i++];
        colors[i][2] = raw_colors[raw_colors_i++];
        if (raw_labels.size() == num_vertices)
            vertex_labels[i] = raw_labels[i];
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
    colormap.resize(num_labels);
    std::fill(colormap.begin(), colormap.end(), Eigen::Vector3i(250, 0, 0));
    
    std::vector<uint16_t> labelmap;
    for (int i = 0; i < colors.size(); i++) {
        if (std::find(labelmap.begin(), labelmap.end(), labels[i]) == labelmap.end()) {
            // new label found
            const int label = labels[i];
            labelmap.push_back(label);
            if (label > colormap.size()) {
                std::cout << "skip label in ply (index above threshold)." << std::endl;
                continue;
            }
            colormap[label] = colors[i];
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
    classes.resize(colors.size());
    CreateColorMap(colors, colormap);
    
    int vertex_class_i = 0;
    for (const auto& color: colors) {
        int class_i;
        for (class_i = 0; class_i < colormap.size(); class_i++) {
            if (colormap[class_i] == color) {
                break;
            }
        }
        classes[vertex_class_i++] = class_i+1;
    }
    return true;
}

void ClassyVoxelizer::GetVoxelSpaceDimensions(const double voxel_size) {
    Eigen::Vector3f centroid(0,0,0);
    
    for (const Eigen::Vector3f& vertex: vertices)
        centroid += vertex;
    centroid /= vertices.size();
    
    max = centroid;
    min = centroid;
    
    for (const Eigen::Vector3f& vertex: vertices) {
        if ((vertex[0] - centroid[0]) > (max[0] - centroid[0]))
            max[0] = vertex[0];
        
        if ((vertex[1] - centroid[1]) > (max[1] - centroid[1]))
            max[1] = vertex[1];
        
        if ((vertex[2] - centroid[2]) > (max[2] - centroid[2]))
            max[2] = vertex[2];
        
        if ((vertex[0] - centroid[0]) < (min[0] - centroid[0]))
            min[0] = vertex[0];
        
        if ((vertex[1] - centroid[1]) < (min[1] - centroid[1]))
            min[1] = vertex[1];
        
        if ((vertex[2] - centroid[2]) < (min[2] - centroid[2]))
            min[2] = vertex[2];
    }
    
    max += Eigen::Vector3f(voxel_size, voxel_size, voxel_size);
    min -= Eigen::Vector3f(voxel_size, voxel_size, voxel_size);
}
