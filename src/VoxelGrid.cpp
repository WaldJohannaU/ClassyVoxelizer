/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "VoxelGrid.h"
#include <fstream>

VoxelGridInterface::VoxelGridInterface(const Eigen::Vector3f& grid_min,
                                       const Eigen::Vector3f& grid_max,
                                       float voxel_size):
    grid_min_(grid_min), grid_max_(grid_max), voxel_size_(voxel_size) {
    const Eigen::Vector3f grid_size = grid_max - grid_min;
    voxels_per_dim_ = (grid_size / voxel_size).cast<int>();
    num_voxels_ = voxels_per_dim_.prod();
}

bool VoxelGridInterface::IsVoxelOccupied(const Eigen::Vector3f& vertex) const {
    const uint32_t voxel_id = GetEnclosingVoxelID(vertex);
    if (voxel_id == -1)
        return false;
    return IsVoxelOccupied(voxel_id);
}

unsigned int VoxelGridInterface::GetNumOccupied() const {
    unsigned int numOccupied = 0;
    for (int i = 0; i < voxels_per_dim_[0]; i++) {
        for (int j = 0; j < voxels_per_dim_[1]; j++) {
            for (int k = 0; k < voxels_per_dim_[2]; k++) {
                const uint32_t voxel_id = static_cast<unsigned int>(voxels_per_dim_[0] *
                                                                    voxels_per_dim_[1] * k +
                                                                    voxels_per_dim_[0] * j + i);
                if (IsVoxelOccupied(voxel_id))
                    numOccupied++;
            }
        }
    }
    return numOccupied;
}

unsigned int VoxelGridInterface::GetEnclosingVoxelID(const Eigen::Vector3f& vertex) const {
    if (vertex[0] < grid_min_[0] ||
        vertex[1] < grid_min_[1] ||
        vertex[2] < grid_min_[2] ||
        vertex[0] > grid_max_[0] ||
        vertex[1] > grid_max_[1] ||
        vertex[2] > grid_max_[2])
        return -1;
    
    const Eigen::Vector3f vertex_offset_discretized = (vertex - grid_min_) / voxel_size_;
    const float result = voxels_per_dim_[0] * voxels_per_dim_[1] *
                         std::floor(vertex_offset_discretized[2]) + voxels_per_dim_[0] *
                         std::floor(vertex_offset_discretized[1]) + std::floor(vertex_offset_discretized[0]);
    return static_cast<unsigned int>(result);
}

const Eigen::Vector3i& VoxelGridInterface::GetVoxelsPerDim() const {
    return voxels_per_dim_;
}

void VoxelGridInterface::SaveAsPLY(const std::string& filepath) const {
    const unsigned int num_occupied_voxels = GetNumOccupiedVoxels();
    std::vector<float> vertices(num_occupied_voxels * 3);
    std::vector<uint8_t> colors(num_occupied_voxels * 4);
    std::vector<int> labels(num_occupied_voxels * 4);
    unsigned int raw_vertex_i = 0;
    unsigned int raw_color_i = 0;
    unsigned int raw_label_i = 0;
    for (int i = 0; i < voxels_per_dim_[0]; i++) {
        for (int j = 0; j < voxels_per_dim_[1]; j++) {
            for (int k = 0; k < voxels_per_dim_[2]; k++) {
                const uint32_t voxel_id = static_cast<unsigned int>(voxels_per_dim_[0] *
                                                                    voxels_per_dim_[1] * k +
                                                                    voxels_per_dim_[0] * j + i);
                if (IsVoxelOccupied(voxel_id)) {
                    const Eigen::Vector3i& color = GetVoxelColor(voxel_id);
                    int label = GetVoxelClass(voxel_id);
                    Eigen::Vector3f voxel_pos((i * voxel_size_) + grid_min_[0] + voxel_size_ / 2,
                                              (j * voxel_size_) + grid_min_[1] + voxel_size_ / 2,
                                              (k * voxel_size_) + grid_min_[2] + voxel_size_ / 2);
                    vertices[raw_vertex_i++] = voxel_pos[0];
                    vertices[raw_vertex_i++] = voxel_pos[1];
                    vertices[raw_vertex_i++] = voxel_pos[2];
                    colors[raw_color_i++] = color[0];
                    colors[raw_color_i++] = color[1];
                    colors[raw_color_i++] = color[2];
                    colors[raw_color_i++] = 255;
                    labels[raw_label_i++] = label;
                }
            }
        }
    }
    std::filebuf fb;
    fb.open(filepath, std::ios::out | std::ios::binary);
    std::ostream ss(&fb);
    tinyply::PlyFile out_file;
    out_file.add_properties_to_element("vertex", { "x", "y", "z" }, vertices);
    out_file.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, colors);
    out_file.add_properties_to_element("vertex", { "label" }, labels);
    out_file.write(ss, true);
    fb.close();
}

int VoxelGridInterface::GetVoxelClass(const uint32_t voxel_id) const {
    return 0;
}
