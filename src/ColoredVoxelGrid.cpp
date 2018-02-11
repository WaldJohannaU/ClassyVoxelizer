/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "ColoredVoxelGrid.h"

ColoredVoxelGrid::ColoredVoxelGrid(const Eigen::Vector3f& grid_min,
                                   const Eigen::Vector3f& grid_max,
                                   float voxel_size):
    VoxelGridInterface(grid_min, grid_max, voxel_size), empty_voxel_(-1,-1,-1) {
    voxel_grid_.resize(num_voxels_, empty_voxel_);
}

bool ColoredVoxelGrid::IsVoxelOccupied(uint32_t voxel_id) const {
	return voxel_grid_[voxel_id][0] != empty_voxel_[0] ||
           voxel_grid_[voxel_id][1] != empty_voxel_[1] ||
           voxel_grid_[voxel_id][2] != empty_voxel_[2];
}

void ColoredVoxelGrid::SetVoxelColor(const uint32_t voxel_id, const Eigen::Vector3i& color) {
    if (voxel_id < num_voxels_)
        voxel_grid_[voxel_id] = color;
}

const Eigen::Vector3i& ColoredVoxelGrid::GetVoxelColor(const uint32_t voxel_id) const {
    if (voxel_id < num_voxels_)
        return voxel_grid_[voxel_id];
    return empty_voxel_;
}

unsigned int ColoredVoxelGrid::GetNumOccupiedVoxels() const {
    unsigned int num_occupied_voxels = 0;
    for (const auto& voxel: voxel_grid_) {
        if (voxel != Eigen::Vector3i(-1, -1, -1))
            num_occupied_voxels++;
    }
    return num_occupied_voxels;
}

void ColoredVoxelGrid::SetVoxelColor(const Eigen::Vector3f& vertex, const Eigen::Vector3i& color) {
    const uint32_t voxel_id = GetEnclosingVoxelID(vertex);
    SetVoxelColor(voxel_id, color);
}
