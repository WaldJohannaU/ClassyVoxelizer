/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "MultiClassVoxelGrid.h"

MultiClassVoxelGrid::MultiClassVoxelGrid(const Eigen::Vector3f& grid_min,
                                         const Eigen::Vector3f& grid_max, float voxel_size):
    VoxelGridInterface(grid_min, grid_max, voxel_size) {
    voxel_grid_.resize(num_voxels_, 0);
}

bool MultiClassVoxelGrid::IsVoxelOccupied(const uint32_t voxel_id) const {
	return voxel_grid_[voxel_id] != 0;
}

void MultiClassVoxelGrid::SetVoxelClass(const uint32_t voxel_id, const uint8_t class_i) {
    if (voxel_id < num_voxels_)
        voxel_grid_[voxel_id] = class_i;
}

int MultiClassVoxelGrid::GetVoxelClass(const uint32_t voxel_id) const {
    if (voxel_id < num_voxels_)
        return voxel_grid_[voxel_id];
    else
        return -1;
    return (voxel_id < num_voxels_) ? voxel_grid_[voxel_id] : -1;
}

unsigned int MultiClassVoxelGrid::GetNumOccupiedVoxels() const {
    unsigned int num_occupied_voxels = 0;
    for (const auto& voxel: voxel_grid_) {
        if (voxel != 0)
            num_occupied_voxels++;
    }
    return num_occupied_voxels;
}

const Eigen::Vector3i& MultiClassVoxelGrid::GetVoxelColor(const uint32_t voxel_id) const {
    const int class_i = GetVoxelClass(voxel_id);
    return class_color_mapping[class_i];
}

