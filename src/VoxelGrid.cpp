/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */
#include "VoxelGrid.h"

VoxelGridInterface::VoxelGridInterface(const Eigen::Vector3f& grid_min,
                                       const Eigen::Vector3f& grid_max,
                                       float voxel_size) {
    _grid_min = grid_min;
    _grid_max = grid_max;
    _voxel_size = voxel_size;
    _grid_size = grid_max - grid_min;
    _voxels_per_dim = (_grid_size / voxel_size).cast<int>();
    _num_voxels = _voxels_per_dim.prod();
}

bool VoxelGridInterface::IsVoxelOccupied(const Eigen::Vector3f& vertex) const {
    const uint32_t voxel_id = GetEnclosingVoxelID(vertex);
    if (voxel_id == -1)
        return false;
    return IsVoxelOccupied(voxel_id);
}

unsigned int VoxelGridInterface::GetNumOccupied() const {
    unsigned int numOccupied = 0;
    for (int i = 0; i < _voxels_per_dim[0]; i++) {
        for (int j = 0; j < _voxels_per_dim[1]; j++) {
            for (int k = 0; k < _voxels_per_dim[2]; k++) {
                
                uint32_t voxel_id = (unsigned int) _voxels_per_dim[0] * _voxels_per_dim[1] * k + _voxels_per_dim[0] * j + i;
                
                if (IsVoxelOccupied(voxel_id))
                    numOccupied++;
            }
        }
    }
    return numOccupied;
}

unsigned int VoxelGridInterface::GetEnclosingVoxelID(const Eigen::Vector3f& vertex) const {
    if (vertex[0] < _grid_min[0] ||
        vertex[1] < _grid_min[1] ||
        vertex[2] < _grid_min[2] ||
        vertex[0] > _grid_max[0] ||
        vertex[1] > _grid_max[1] ||
        vertex[2] > _grid_max[2])
        return -1;
    
    Eigen::Vector3f vertex_offset_discretized = (vertex - _grid_min) / _voxel_size;
    const float result = _voxels_per_dim[0] * _voxels_per_dim[1] *
                         std::floor(vertex_offset_discretized[2]) + _voxels_per_dim[0] *
                         std::floor(vertex_offset_discretized[1]) + std::floor(vertex_offset_discretized[0]);
    return static_cast<unsigned int>(result);
}

const Eigen::Vector3i& VoxelGridInterface::GetVoxelsPerDim() const {
    return _voxels_per_dim;
}
