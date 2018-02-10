/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __VOXELIZER__
#define __VOXELIZER__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <string>
#include <Eigen/Dense>

#include "VoxelGrid.h"

class Voxelizer {
public:
protected:
    const float kVoxelizerMinTriangleArea = 0.00001;
    virtual Eigen::Vector3f GetMidpoint(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) const;
    virtual const float EuclideanDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) const;
    virtual const float AreaOfTriangle(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const Eigen::Vector3f& v3) const;
    virtual int SplitBaseFace(const VoxelGridInterface& voxel_grid,
                              std::vector<Eigen::Vector3f>& vertices,
                              std::vector<uint32_t>& face,
                              std::vector<uint32_t>& sub_faces,
                              std::vector<uint32_t>& first_sub_face,
                              std::vector<uint32_t>& second_sub_face) const;

public:
};

#endif /* defined(__MULTICLASSVOXELIZER__) */
