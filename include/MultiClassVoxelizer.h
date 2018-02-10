/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __MULTICLASSVOXELIZER__
#define __MULTICLASSVOXELIZER__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <string>

//Eigen
#include <Eigen/Dense>

#include "Voxelizer.h"
#include "MultiClassVoxelGrid.h"

class MultiClassVoxelizer: public Voxelizer {
public:
    void Voxelize(MultiClassVoxelGrid& voxel_grid,
                  std::vector<Eigen::Vector3f>& vertices,
                  std::vector<uint32_t>& faces,
                  std::vector<uint16_t>& vertex_classes);
private:
    virtual void SplitFace(MultiClassVoxelGrid& voxel_grid,
                   std::vector<Eigen::Vector3f>& vertices,
                   std::vector<uint16_t>& vertex_classes,
                   std::vector<uint32_t>& face,
                   std::vector<uint32_t>& sub_faces);
};

#endif /* defined(__MULTICLASSVOXELIZER__) */
