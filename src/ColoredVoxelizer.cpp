/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "ColoredVoxelizer.h"

void ColoredVoxelizer::Voxelize(ColoredVoxelGrid& voxel_grid,
                                            std::vector<Eigen::Vector3f>& vertices,
                                            std::vector<uint32_t> &faces,
                                            std::vector<Eigen::Vector3i> &colors) {
    std::vector<uint32_t> split_faces;
    const int ten_percent_step = faces.size() / 10;
    std::vector<uint32_t> face(3);
    for (int i = 0; i < faces.size(); i+=3) {
        face[0] = faces[i];
        face[1] = faces[i+1];
        face[2] = faces[i+2];
		std::vector<uint32_t> sub_faces;
		SplitFace(voxel_grid, vertices, colors, face, sub_faces);
        split_faces.insert(split_faces.end(), sub_faces.begin(), sub_faces.end());
        if ((i % ten_percent_step == 0 || (i-1) % ten_percent_step == 0 || (i-2) % ten_percent_step == 0) && i != 0)
            std::cout << i / ten_percent_step << "0% " << std::flush;
    }
    for (auto& split_face_vertex_i : split_faces) {
        voxel_grid.SetVoxelColor(vertices[split_face_vertex_i], colors[split_face_vertex_i]);
    }
    std::cout << "100%" << std::endl;
}

void ColoredVoxelizer::SplitFace(ColoredVoxelGrid& voxel_grid,
                                 std::vector<Eigen::Vector3f>& vertices,
                                 std::vector<Eigen::Vector3i>& colors,
                                 std::vector<uint32_t>& face,
                                 std::vector<uint32_t>& sub_faces) {
    std::vector<uint32_t> first_sub_face(3);
    std::vector<uint32_t> second_sub_face(3);
    
    const int longest_i = SplitBaseFace(voxel_grid, vertices, face, sub_faces,
                                        first_sub_face, second_sub_face);
    if (longest_i == -1)
        return;
    
	colors.push_back((colors[face[longest_i % 3]] + colors[face[(longest_i + 1) % 3]]) / 2);
    
    SplitFace(voxel_grid, vertices, colors, first_sub_face, sub_faces);
    SplitFace(voxel_grid, vertices, colors, second_sub_face, sub_faces);
}
