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
    ClassyVoxelizer(const float voxel_size);
    void Process(const VoxelType voxel_type,
                 const std::string& input_file,
                 const std::string& output_file,
                 const std::string& mesh_output);
private:
    const float voxel_size_ = 0;
    const int num_labels_ = 1163; // ScanNet
    Eigen::Vector3f min_;
    Eigen::Vector3f max_;
    
    std::vector<Eigen::Vector3f> vertices_;
    std::vector<uint32_t> faces_;
    std::vector<uint16_t> vertex_classes_;
    std::vector<uint16_t> vertex_labels_;
    std::vector<Eigen::Vector3i> colormap_;
    std::vector<Eigen::Vector3i> colors_;

    bool SaveVoxelizedMesh(const std::string& file_name) const;
    int ReadPly(const std::string& filepath);
    bool CreateColorMap(const std::vector<Eigen::Vector3i>& colors,
                        std::vector<Eigen::Vector3i>& colormap);
    bool ComputeColorFromLabel(std::vector<uint16_t>& labels, const int num_labels);
    bool ComputeClassFromColor(std::vector<uint16_t>& classes);
    void GetVoxelSpaceDimensions(const double voxel_size);
};

#endif /* defined(__MULTICLASSVOXELIZER__) */
