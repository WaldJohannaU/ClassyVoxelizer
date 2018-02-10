/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "Voxelizer.h"

Eigen::Vector3f Voxelizer::GetMidpoint(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) const {
    return (v1 + v2) / 2;
}

const float Voxelizer::EuclideanDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) const {
    return std::sqrt(std::pow((v1[0] - v2[0]),2) + std::pow((v1[1] - v2[1]),2) + std::pow((v1[2] - v2[2]),2));
}

const float Voxelizer::AreaOfTriangle(const Eigen::Vector3f& v1,
                                      const Eigen::Vector3f& v2,
                                      const Eigen::Vector3f& v3) const {
    return (v2 - v1).cross(v3 - v1).norm() / 2.0;
}

int Voxelizer::SplitBaseFace(const VoxelGridInterface& voxel_grid,
                          std::vector<Eigen::Vector3f>& vertices,
                          std::vector<uint32_t>& face,
                          std::vector<uint32_t>& sub_faces,
                          std::vector<uint32_t>& first_sub_face,
                          std::vector<uint32_t>& second_sub_face) const {
    if (AreaOfTriangle(vertices[face[0]], vertices[face[1]], vertices[face[2]]) < kVoxelizerMinTriangleArea) {
        sub_faces.insert(sub_faces.end(), face.begin(), face.end());
        return -1;
    }
    
    std::vector<double> side_lengths(3,0);
    bool single_voxel_triangle = true;
    for (int i = 0; i < 3; i++) {
        if (voxel_grid.GetEnclosingVoxelID(vertices[face[i % 3]]) != voxel_grid.GetEnclosingVoxelID(vertices[face[(i + 1) % 3]])) {
            side_lengths[i] = EuclideanDistance(vertices[face[i % 3]], vertices[face[(i + 1) % 3]]);
            single_voxel_triangle = false;
        }
    }
    
    if (single_voxel_triangle) {
        sub_faces.insert(sub_faces.end(), face.begin(), face.end());
        return -1;
    }
    
    int longest_i = 0;
    double longest_length = 0;
    for (int i = 0; i < 3; i++) {
        if (side_lengths[i] > longest_length) {
            longest_length = side_lengths[i];
            longest_i = i;
        }
    }
    
    Eigen::Vector3f new_midpoint = GetMidpoint(vertices[face[longest_i % 3]], vertices[face[(longest_i + 1) % 3]]);
    vertices.push_back(new_midpoint);
    
    first_sub_face[0] = face[longest_i % 3];
    first_sub_face[1] = face[(longest_i + 2) % 3];
    first_sub_face[2] = vertices.size()-1;
    
    second_sub_face[0] = face[(longest_i + 1) % 3];
    second_sub_face[1] = face[(longest_i + 2) % 3];
    second_sub_face[2] = vertices.size()-1;
    
    return longest_i;
}

