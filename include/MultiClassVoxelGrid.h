/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __MULTICLASSVOXELGRID__
#define __MULTICLASSVOXELGRID__

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

//Eigen
#include <Eigen/Dense>
#include "VoxelGrid.h"

#include "tinyply.h"

class MultiClassVoxelGrid: public VoxelGridInterface {
public:
    MultiClassVoxelGrid(const Eigen::Vector3f& grid_min,
                        const Eigen::Vector3f& grid_max,
                        float voxel_size);
    void SetVoxelClass(const uint32_t voxel_id, const uint8_t class_i);
    std::vector<Eigen::Vector3i> class_color_mapping;
private:
    std::vector<uint8_t> voxel_grid_;
    virtual bool IsVoxelOccupied(const uint32_t voxel_id) const override;
    virtual int GetVoxelClass(const uint32_t voxel_id) const override;
    virtual const Eigen::Vector3i& GetVoxelColor(const uint32_t voxel_id) const override;
    virtual unsigned int GetNumOccupiedVoxels() const override;
};

#endif /* defined(__MULTICLASSVOXELGRID__) */
