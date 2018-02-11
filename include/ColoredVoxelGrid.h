/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __ColoredVOXELGRID__
#define __ColoredVOXELGRID__

#include <vector>

//Eigen
#include <Eigen/Dense>
#include "VoxelGrid.h"

#include "tinyply.h"

class ColoredVoxelGrid: public VoxelGridInterface {
public:
    ColoredVoxelGrid(const Eigen::Vector3f& grid_min,
                     const Eigen::Vector3f& grid_max,
                     const float voxel_size);
    void SetVoxelColor(const Eigen::Vector3f& vertex, const Eigen::Vector3i& color);
private:
    std::vector<Eigen::Vector3i> voxel_grid_;
    const Eigen::Vector3i empty_voxel_;
    void SetVoxelColor(const uint32_t voxel_id, const Eigen::Vector3i& color);
    virtual bool IsVoxelOccupied(const uint32_t voxel_id) const override;
    virtual const Eigen::Vector3i& GetVoxelColor(const uint32_t voxel_id) const override;

    virtual unsigned int GetNumOccupiedVoxels() const override;
};

#endif /* defined(__ColoredVOXELGRID__) */
