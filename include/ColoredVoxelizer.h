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

#include "Voxelizer.h"
#include "ColoredVoxelGrid.h"

class ColoredVoxelizer: public Voxelizer {
public:
    void Voxelize(ColoredVoxelGrid& voxel_grid,
                  std::vector<Eigen::Vector3f>& vertices,
                  std::vector<uint32_t>& faces,
                  std::vector<Eigen::Vector3i>& colors);
private:
	void SplitFace(ColoredVoxelGrid& voxel_grid,
                   std::vector<Eigen::Vector3f>& vertices,
                   std::vector<Eigen::Vector3i>& colors,
                   std::vector<uint32_t>& face,
                   std::vector<uint32_t>& sub_faces);
       
};

#endif /* defined(__ColoredVOXELIZER__) */
