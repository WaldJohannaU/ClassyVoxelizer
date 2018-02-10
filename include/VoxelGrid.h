/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __VOXELGRID__
#define __VOXELGRID__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

//Eigen
#include <Eigen/Dense>
#include "tinyply.h"

class VoxelGridInterface {
public:
    VoxelGridInterface(const Eigen::Vector3f& grid_min,
                       const Eigen::Vector3f& grid_max,
                       float voxel_size);
    virtual const Eigen::Vector3i& GetVoxelsPerDim() const;

    virtual void SaveAsRAW(const std::string& filepath) const = 0;
    virtual uint32_t GetEnclosingVoxelID(const Eigen::Vector3f& vertex) const;
protected:
    Eigen::Vector3i _voxels_per_dim;
    Eigen::Vector3f _grid_min;
    Eigen::Vector3f _grid_max;
    Eigen::Vector3f _grid_size;
    float _voxel_size;
    
    uint32_t _num_voxels;
    virtual unsigned int GetNumOccupied() const;
private:
    virtual bool IsVoxelOccupied(const uint32_t voxel_id) const = 0;
    virtual bool IsVoxelOccupied(const Eigen::Vector3f& vertex) const;
};

#endif /* defined(__ColoredVOXELGRID__) */
