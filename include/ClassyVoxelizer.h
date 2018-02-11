/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#ifndef __CLASSYVOXELIZER__
#define __CLASSYVOXELIZER__

#include <Eigen/Dense>
#include <vector>

enum class VoxelType {
    color, label
};

class ClassyVoxelizer {
public:
    void Process(const VoxelType voxel_type,
                 const std::string& input_file,
                 const std::string& output_file,
                 const float voxel_size,
                 const std::string& mesh_output);
private:
    const int num_labels = 1163; // ScanNet
    Eigen::Vector3f min;
    Eigen::Vector3f max;
    
    std::vector<Eigen::Vector3f> vertices;
    std::vector<uint32_t> faces;
    std::vector<uint16_t> vertex_classes;
    std::vector<uint16_t> vertex_labels;
    std::vector<Eigen::Vector3i> colormap;
    std::vector<Eigen::Vector3i> colors;

    int ReadPly(const std::string& filepath);
    bool CreateColorMap(const std::vector<Eigen::Vector3i>& colors,
                        std::vector<Eigen::Vector3i>& colormap);
    bool ComputeColorFromLabel(std::vector<uint16_t>& labels, const int num_labels);
    bool ComputeClassFromColor(std::vector<uint16_t>& classes);
    void GetVoxelSpaceDimensions(const double voxel_size);
};

#endif /* defined(__MULTICLASSVOXELIZER__) */
