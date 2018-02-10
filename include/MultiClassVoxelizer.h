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

#include "MultiClassVoxelGrid.h"

constexpr float kVoxelizerMinTriangleArea = 0.00001;

class MultiClassVoxelizer {
public:
    static MultiClassVoxelGrid Voxelize(std::vector<Eigen::Vector3f>& vertices,
                                        std::vector<uint32_t>& faces,
                                        Eigen::Vector3f& grid_min,
                                        Eigen::Vector3f& grid_max, float voxel_size,
                                        std::vector<uint16_t>& vertex_classes);
private:
    static Eigen::Vector3f GetMidpoint(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);

    static float EuclideanDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);
    
    static float AreaOfTriangle(const Eigen::Vector3f& vertex_1,
                                const Eigen::Vector3f& vertex_2,
                                const Eigen::Vector3f& vertex_3);
    
	static void SplitFace(MultiClassVoxelGrid& voxel_grid,
                          std::vector<Eigen::Vector3f>& vertices,
                          std::vector<uint16_t>& vertex_classes,
                          std::vector<uint32_t>& face,
                          std::vector<uint32_t>& sub_faces);
};

#endif /* defined(__MULTICLASSVOXELIZER__) */
