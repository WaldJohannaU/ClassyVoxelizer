/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __ColoredVOXELGRID__
#define __ColoredVOXELGRID__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

//Eigen
#include <Eigen/Dense>
#include "VoxelGrid.h"

#include "tinyply.h"

class ColoredVoxelGrid: public VoxelGridInterface {
public:
    ColoredVoxelGrid(const Eigen::Vector3f& grid_min,
                     const Eigen::Vector3f& grid_max,
                     float voxel_size);

    void SaveAsPLY(const std::string& filepath) const;
    void SetVoxelColor(const Eigen::Vector3f& vertex, const Eigen::Vector3i& color);

private:
    std::vector<Eigen::Vector3i> _voxelgrid;
    void SetVoxelColor(const uint32_t voxel_id, const Eigen::Vector3i& color);

    virtual bool IsVoxelOccupied(const uint32_t voxel_id) const override;
    std::vector<Eigen::Vector3i> GetVoxelGrid();
    
    Eigen::Vector3i GetVoxelColor(const uint32_t voxel_id) const;
    const std::vector<Eigen::Vector3i>& GetVoxelGrid() const;
};

#endif /* defined(__ColoredVOXELGRID__) */
