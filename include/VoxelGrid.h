/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __VOXELGRID__
#define __VOXELGRID__

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
    virtual uint32_t GetEnclosingVoxelID(const Eigen::Vector3f& vertex) const;
    void SaveAsPLY(const std::string& filepath) const;
    void SaveAsPLYMesh(const std::string& filepath) const;
protected:
    Eigen::Vector3i voxels_per_dim_;
    const Eigen::Vector3f grid_min_;
    const Eigen::Vector3f grid_max_;
    const float voxel_size_;
    uint32_t num_voxels_;
    
    virtual unsigned int GetNumOccupied() const;
private:
    virtual bool IsVoxelOccupied(const uint32_t voxel_id) const = 0;
    virtual bool IsVoxelOccupied(const Eigen::Vector3f& vertex) const;
    
    virtual unsigned int GetNumOccupiedVoxels() const = 0;
    virtual const Eigen::Vector3i& GetVoxelColor(const uint32_t voxel_id) const = 0;
    virtual int GetVoxelClass(const uint32_t voxel_id) const;
    
    void WritePlyHeader(std::ofstream& file_out_, const int vertex, const int faces) const;
    void WriteVertex(std::stringstream& vertices_,
                     const Eigen::Vector3f& pos, const Eigen::Vector3i& color) const;
    void WriteFace(std::stringstream& faces_,
                                       const int index1, const int index2, const int index3) const;
    
    /*void WriteFace(std::vector<uint32_t>& local_faces, int& index,
     const int v1, const int v2, const int v3) const;
     
     void WriteVertex(std::vector<float>& local_vertices,
     std::vector<uint8_t>& local_colors,
     const Eigen::Vector3f vertex,
     const Eigen::Vector3i color,
     int& color_index,
     int& vertex_index) const;*/
};

#endif /* defined(__ColoredVOXELGRID__) */
