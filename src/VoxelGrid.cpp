/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "VoxelGrid.h"
#include <fstream>

VoxelGridInterface::VoxelGridInterface(const Eigen::Vector3f& grid_min,
                                       const Eigen::Vector3f& grid_max,
                                       float voxel_size):
    grid_min_(grid_min), grid_max_(grid_max), voxel_size_(voxel_size) {
    const Eigen::Vector3f grid_size = grid_max - grid_min;
    voxels_per_dim_ = (grid_size / voxel_size).cast<int>();
    num_voxels_ = voxels_per_dim_.prod();
}

bool VoxelGridInterface::IsVoxelOccupied(const Eigen::Vector3f& vertex) const {
    const uint32_t voxel_id = GetEnclosingVoxelID(vertex);
    if (voxel_id == -1)
        return false;
    return IsVoxelOccupied(voxel_id);
}

unsigned int VoxelGridInterface::GetNumOccupied() const {
    unsigned int numOccupied = 0;
    for (int i = 0; i < voxels_per_dim_[0]; i++) {
        for (int j = 0; j < voxels_per_dim_[1]; j++) {
            for (int k = 0; k < voxels_per_dim_[2]; k++) {
                const uint32_t voxel_id = static_cast<unsigned int>(voxels_per_dim_[0] *
                                                                    voxels_per_dim_[1] * k +
                                                                    voxels_per_dim_[0] * j + i);
                if (IsVoxelOccupied(voxel_id))
                    numOccupied++;
            }
        }
    }
    return numOccupied;
}

unsigned int VoxelGridInterface::GetEnclosingVoxelID(const Eigen::Vector3f& vertex) const {
    if (vertex[0] < grid_min_[0] ||
        vertex[1] < grid_min_[1] ||
        vertex[2] < grid_min_[2] ||
        vertex[0] > grid_max_[0] ||
        vertex[1] > grid_max_[1] ||
        vertex[2] > grid_max_[2])
        return -1;
    
    const Eigen::Vector3f vertex_offset_discretized = (vertex - grid_min_) / voxel_size_;
    const float result = voxels_per_dim_[0] * voxels_per_dim_[1] *
                         std::floor(vertex_offset_discretized[2]) + voxels_per_dim_[0] *
                         std::floor(vertex_offset_discretized[1]) + std::floor(vertex_offset_discretized[0]);
    return static_cast<unsigned int>(result);
}

const Eigen::Vector3i& VoxelGridInterface::GetVoxelsPerDim() const {
    return voxels_per_dim_;
}

void VoxelGridInterface::SaveAsPLY(const std::string& filepath) const {
    const unsigned int num_occupied_voxels = GetNumOccupiedVoxels();
    std::vector<float> vertices(num_occupied_voxels * 3);
    std::vector<uint8_t> colors(num_occupied_voxels * 4);
    std::vector<int> labels(num_occupied_voxels * 4);
    unsigned int raw_vertex_i = 0;
    unsigned int raw_color_i = 0;
    unsigned int raw_label_i = 0;
    for (int i = 0; i < voxels_per_dim_[0]; i++) {
        for (int j = 0; j < voxels_per_dim_[1]; j++) {
            for (int k = 0; k < voxels_per_dim_[2]; k++) {
                const uint32_t voxel_id = static_cast<unsigned int>(voxels_per_dim_[0] *
                                                                    voxels_per_dim_[1] * k +
                                                                    voxels_per_dim_[0] * j + i);
                if (IsVoxelOccupied(voxel_id)) {
                    const Eigen::Vector3i& color = GetVoxelColor(voxel_id);
                    int label = GetVoxelClass(voxel_id);
                    Eigen::Vector3f voxel_pos((i * voxel_size_) + grid_min_[0] + voxel_size_ / 2,
                                              (j * voxel_size_) + grid_min_[1] + voxel_size_ / 2,
                                              (k * voxel_size_) + grid_min_[2] + voxel_size_ / 2);
                    vertices[raw_vertex_i++] = voxel_pos[0];
                    vertices[raw_vertex_i++] = voxel_pos[1];
                    vertices[raw_vertex_i++] = voxel_pos[2];
                    colors[raw_color_i++] = color[0];
                    colors[raw_color_i++] = color[1];
                    colors[raw_color_i++] = color[2];
                    colors[raw_color_i++] = 255;
                    labels[raw_label_i++] = label;
                }
            }
        }
    }
    std::filebuf fb;
    fb.open(filepath, std::ios::out | std::ios::binary);
    std::ostream ss(&fb);
    tinyply::PlyFile out_file;
    out_file.add_properties_to_element("vertex", { "x", "y", "z" }, vertices);
    out_file.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, colors);
    out_file.add_properties_to_element("vertex", { "label" }, labels);
    out_file.write(ss, true);
    fb.close();
}


/*void ClassyVoxelizer::WriteFace(std::vector<uint32_t>& local_faces,
 int& index,
 const int v1, const int v2, const int v3) const {
 local_faces[index++] = v1;
 local_faces[index++] = v2;
 local_faces[index++] = v3;
 }
 
 void ClassyVoxelizer::WriteVertex(
 std::vector<float>& local_vertices,
 std::vector<uint8_t>& local_colors,
 const Eigen::Vector3f vertex,
 const Eigen::Vector3i color,
 int& color_index,
 int& vertex_index) const {
 
 local_colors[color_index++] = static_cast<uint8_t>(color[0]);
 local_colors[color_index++] = static_cast<uint8_t>(color[1]);
 local_colors[color_index++] = static_cast<uint8_t>(color[2]);
 local_colors[color_index++] = 255;
 
 local_vertices[vertex_index++] = static_cast<float>(vertex[0]);
 local_vertices[vertex_index++] = static_cast<float>(vertex[1]);
 local_vertices[vertex_index++] = static_cast<float>(vertex[2]);
 }*/

void VoxelGridInterface::WritePlyHeader(std::ofstream& file_out_, const int vertex, const int faces) const {
    file_out_ << "ply" << std::endl;
    file_out_ << "format ascii 1.0" << std::endl;
    file_out_ << "element vertex " << vertex << std::endl;
    file_out_ << "property float x" << std::endl;
    file_out_ << "property float y" << std::endl;
    file_out_ << "property float z" << std::endl;
    file_out_ << "property float nx" << std::endl;
    file_out_ << "property float ny" << std::endl;
    file_out_ << "property float nz" << std::endl;
    file_out_ << "property uchar red" << std::endl;
    file_out_ << "property uchar green" << std::endl;
    file_out_ << "property uchar blue" << std::endl;
    file_out_ << "element face " << faces << std::endl;
    file_out_ << "property list uchar int vertex_indices" << std::endl;
    file_out_ << "end_header" << std::endl;
}

void VoxelGridInterface::WriteVertex(std::stringstream& vertices_,
                                     const Eigen::Vector3f& pos, const Eigen::Vector3i& color) const {
    vertices_ << pos[0] << " " << pos[1] << " " << pos[2] << " 0 0 0 " <<
                 color[0] << " " << color[1] << " " << color[2] << std::endl;
}

void VoxelGridInterface::WriteFace(std::stringstream& faces_,
                                const int index1, const int index2, const int index3) const {
    faces_ << "3 " << index1 << " " << index2 << " " << index3 << std::endl;
}

void VoxelGridInterface::SaveAsPLYMesh(const std::string& filepath) const {
    if (filepath == "")
        return;
    const unsigned int num_occupied_voxels = GetNumOccupiedVoxels();
    std::vector<float> vertices(num_occupied_voxels * 3);
    std::vector<uint8_t> colors(num_occupied_voxels * 4);
    std::vector<int> labels(num_occupied_voxels * 4);
    Eigen::Vector3f point;
    Eigen::Vector3i color;
    std::stringstream faces_;
    std::stringstream vertices_;
    int counter_faces = 0;
    int counter_vertices = 0;
    int s = 0;
    const float voxel_size = voxel_size_ / 2.0f * 0.95f;
    
    std::ofstream file_out_;
    file_out_.open(filepath);
    
    for (int i = 0; i < voxels_per_dim_[0]; i++) {
        for (int j = 0; j < voxels_per_dim_[1]; j++) {
            for (int k = 0; k < voxels_per_dim_[2]; k++) {
                const uint32_t voxel_id = static_cast<unsigned int>(voxels_per_dim_[0] *
                                                                    voxels_per_dim_[1] * k +
                                                                    voxels_per_dim_[0] * j + i);
                if (IsVoxelOccupied(voxel_id)) {
                    const Eigen::Vector3i& color = GetVoxelColor(voxel_id);
                    Eigen::Vector3f point((i * voxel_size_) + grid_min_[0] + voxel_size_ / 2,
                                          (j * voxel_size_) + grid_min_[1] + voxel_size_ / 2,
                                          (k * voxel_size_) + grid_min_[2] + voxel_size_ / 2);
                    const int offset = 8*s;
                    WriteFace(faces_, offset, offset+1, offset+3);
                    WriteFace(faces_, offset+3, offset+2, offset+1);
                    WriteFace(faces_, offset+3, offset+2, offset+6);
                    WriteFace(faces_, offset+6, offset+7, offset+3);
                    WriteFace(faces_, offset+7, offset+6, offset+5);
                    WriteFace(faces_, offset+5, offset+4, offset+7);
                    WriteFace(faces_, offset+4, offset+5, offset+1);
                    WriteFace(faces_, offset+1, offset+0, offset+4);
                    WriteFace(faces_, offset+4, offset+0, offset+3);
                    WriteFace(faces_, offset+3, offset+7, offset+4);
                    WriteFace(faces_, offset+5, offset+6, offset+2);
                    WriteFace(faces_, offset+2, offset+1, offset+5);
                    counter_faces += 12;
                    
                    Eigen::Vector3f p1(-voxel_size + point[0], voxel_size + point[1], voxel_size + point[2]);
                    Eigen::Vector3f p2(-voxel_size + point[0], voxel_size + point[1], -voxel_size + point[2]);
                    Eigen::Vector3f p3(voxel_size + point[0], voxel_size + point[1], -voxel_size + point[2]);
                    
                    WriteVertex(vertices_, p1, color);
                    WriteVertex(vertices_, p2, color);
                    WriteVertex(vertices_, p3, color);
                    WriteVertex(vertices_,
                                Eigen::Vector3f(voxel_size + point[0], voxel_size + point[1], voxel_size + point[2]), color);
                    WriteVertex(vertices_,
                                Eigen::Vector3f(-voxel_size + point[0], -voxel_size + point[1], voxel_size + point[2]), color);
                    WriteVertex(vertices_,
                                Eigen::Vector3f(-voxel_size + point[0], -voxel_size + point[1], -voxel_size + point[2]), color);
                    WriteVertex(vertices_,
                                Eigen::Vector3f(voxel_size + point[0], -voxel_size + point[1], -voxel_size + point[2]), color);
                    WriteVertex(vertices_,
                                Eigen::Vector3f(voxel_size + point[0], -voxel_size + point[1], voxel_size + point[2]), color);
                    counter_vertices += 8;
                    s++;
                }
            }
        }
    }
    WritePlyHeader(file_out_, counter_vertices, counter_faces);
    file_out_ << vertices_.str() << faces_.str();
    file_out_.close();
}

int VoxelGridInterface::GetVoxelClass(const uint32_t voxel_id) const {
    return 0;
}
