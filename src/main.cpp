/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

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

int readPly(const std::string& filepath,
            std::vector<Eigen::Vector3f>& vertices,
            std::vector<uint32_t>& faces,
            std::vector<Eigen::Vector3i>& colors,
            std::vector<uint16_t> labels) {
    std::ifstream ss(filepath, std::ios::binary);
    tinyply::PlyFile input_file(ss);
    std::vector<float> raw_vertices;
    std::vector<uint8_t> raw_colors;
    std::vector<uint16_t> raw_labels;

    uint32_t num_vertices, num_colors, num_faces, num_labels;
    num_vertices = num_colors = num_faces = 0;
    
    num_vertices = input_file.request_properties_from_element("vertex", { "x", "y", "z" }, raw_vertices);
    num_colors = input_file.request_properties_from_element("vertex", { "red", "green", "blue" }, raw_colors);
    num_labels = input_file.request_properties_from_element("vertex", { "label" }, raw_labels);
    num_faces = input_file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
    
    input_file.read(ss);
    
    vertices.resize(num_vertices);
    colors.resize(num_vertices);
    labels.resize(num_vertices);

    int raw_vertices_i = 0;
    int raw_colors_i = 0;
    int label_i = 0;

    for (int i = 0; i < num_vertices; i++) {
        vertices[i][0] = raw_vertices[raw_vertices_i++];
        vertices[i][1] = raw_vertices[raw_vertices_i++];
        vertices[i][2] = raw_vertices[raw_vertices_i++];
        colors[i][0] = raw_colors[raw_colors_i++];
        colors[i][1] = raw_colors[raw_colors_i++];
        colors[i][2] = raw_colors[raw_colors_i++];
        labels[i] = raw_labels[label_i++];
    }
    return num_vertices;
}

bool computeClassFromColor(std::vector<Eigen::Vector3i>& colors,
                           std::vector<uint16_t>& classes,
                           std::vector<Eigen::Vector3i>& colormap) {
    const int num_vertices = colors.size();
    classes.resize(num_vertices);
    for (const auto& color : colors) {
        if(std::find(colormap.begin(), colormap.end(), color) == colormap.end())
            colormap.push_back(color);
    }
    
    if (colormap.size() > 255) {
        std::cerr << "Error: ClassyVoxelizer only supports up to 255 classes." << std::endl;
        return false;
    }
    
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

void getVoxelSpaceDimensions(const std::vector<Eigen::Vector3f>& vertices,
                             const double voxel_size,
                             Eigen::Vector3f& min,
                             Eigen::Vector3f& max) {
    
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


int main (int argc, char* argv[]) {
    if (argc < 5) {
        const std::string usage_message = "\nUsage:\n\n./classyvoxelizer <input> <output> <voxel_size> <class/color>\n";
        std::cout << usage_message << std::endl;
        return 0;
    }
    
    const std::string input_filepath = argv[1];
    const std::string output_filepath = argv[2];
    const double voxel_size = std::stod(argv[3]);
    
    std::vector<Eigen::Vector3f> vertices;
    std::vector<uint32_t> faces;
    std::vector<uint16_t> vertex_classes;
    std::vector<Eigen::Vector3i> colormap;
    std::vector<Eigen::Vector3i> colors;
    std::vector<uint16_t> labels;
    
    // std::unique_ptr<VoxelGridInterface> voxel_grid;
    if (std::string(argv[4]) == "class") {
        readPly(input_filepath, vertices, faces, colors, labels);
        computeClassFromColor(colors, vertex_classes, colormap);
    } else if (std::string(argv[4]) == "color") {
        readPly(input_filepath, vertices, faces, colors, labels);
    }
    
    Eigen::Vector3f min;
    Eigen::Vector3f max;
    getVoxelSpaceDimensions(vertices, voxel_size, min, max);
    std::cout << "Voxelizing at " << voxel_size << "m resolution: " << std::flush;
    
    if (std::string(argv[4]) == "class") {
        MultiClassVoxelGrid voxelgrid =
            MultiClassVoxelizer::Voxelize(vertices, faces, min, max, voxel_size, vertex_classes);
        voxelgrid.SaveAsPLY(output_filepath, colormap);
    } else if (std::string(argv[4]) == "color") {
        ColoredVoxelGrid voxelgrid =
            ColoredVoxelizer::Voxelize(vertices, faces, min, max, voxel_size, colors);
        voxelgrid.SaveAsPLY(output_filepath);
    }
    return 0;
}
