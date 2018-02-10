/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __ColoredVOXELIZER__
#define __ColoredVOXELIZER__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "ColoredVoxelGrid.h"

constexpr double kColoredVoxelizerMinTriangleArea = 0.00001;

class ColoredVoxelizer {
public:
    static ColoredVoxelGrid Voxelize(std::vector<Eigen::Vector3f>& vertices,
                                     std::vector<uint32_t>& faces,
                                     Eigen::Vector3f grid_min, Eigen::Vector3f grid_max, float voxel_size,
                                     std::vector<Eigen::Vector3i>& colors);
    
private:
    static Eigen::Vector3f GetMidpoint(Eigen::Vector3f v1, Eigen::Vector3f v2);
    static float EuclideanDistance(Eigen::Vector3f v1, Eigen::Vector3f v2);
    static float AreaOfTriangle(Eigen::Vector3f vertex_1, Eigen::Vector3f vertex_2, Eigen::Vector3f vertex_3);
	static void SplitFace(ColoredVoxelGrid& voxel_grid,
                          std::vector<Eigen::Vector3f>& vertices,
                          std::vector<Eigen::Vector3i>& colors,
                          std::vector<uint32_t>& face,
                          std::vector<uint32_t>& sub_faces);
       
};

#endif /* defined(__ColoredVOXELIZER__) */
