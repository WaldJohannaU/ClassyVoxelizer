/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "MultiClassVoxelizer.h"

MultiClassVoxelGrid MultiClassVoxelizer::Voxelize(std::vector<Eigen::Vector3f>& vertices,
                                                  std::vector<uint32_t>& faces,
                                                  Eigen::Vector3f& grid_min,
                                                  Eigen::Vector3f& grid_max, float voxel_size,
                                                  std::vector<uint16_t>& vertex_classes) {

    MultiClassVoxelGrid voxel_grid(grid_min, grid_max, voxel_size);
    
    std::vector<uint32_t> split_faces;
    int ten_percent_step = faces.size() / 10;
    
    for (int i = 0; i < faces.size(); i+=3) {
        
        std::vector<uint32_t> face(3);
        face[0] = faces[i];
        face[1] = faces[i+1];
        face[2] = faces[i+2];
        
		std::vector<uint32_t> sub_faces;
		SplitFace(voxel_grid, vertices, vertex_classes, face, sub_faces);
		split_faces.insert(split_faces.end(), sub_faces.begin(), sub_faces.end());

        if ((i % ten_percent_step == 0 || (i-1) % ten_percent_step == 0 || (i-2) % ten_percent_step == 0) && i != 0)
            std::cout << i / ten_percent_step << "0% " << std::flush;
    }
    
    for (auto split_face_vertex_i: split_faces) {
        int voxel_id = voxel_grid.GetEnclosingVoxelID(vertices[split_face_vertex_i]);
        voxel_grid.SetVoxelClass(voxel_id, vertex_classes[split_face_vertex_i]);
    }

    std::cout << "100%" << std::endl;
    return voxel_grid;
}

void MultiClassVoxelizer::SplitFace(MultiClassVoxelGrid& voxel_grid,
                                    std::vector<Eigen::Vector3f>& vertices,
                                    std::vector<uint16_t>& vertex_classes,
                                    std::vector<uint32_t>& face,
                                    std::vector<uint32_t>& sub_faces) {

	if (AreaOfTriangle(vertices[face[0]], vertices[face[1]], vertices[face[2]]) < kVoxelizerMinTriangleArea) {
		sub_faces.insert(sub_faces.end(), face.begin(), face.end());
		return;
	}

    std::vector<double> side_lengths(3,0);
    bool single_voxel_triangle = true;
    for (int i = 0; i < 3; i++) {
		if (voxel_grid.GetEnclosingVoxelID(vertices[face[i % 3]]) != voxel_grid.GetEnclosingVoxelID(vertices[face[(i + 1) % 3]])) {
            side_lengths[i] = EuclideanDistance(vertices[face[i % 3]], vertices[face[(i + 1) % 3]]);
            single_voxel_triangle = false;
        }        
    }

    if (single_voxel_triangle) {
        sub_faces.insert(sub_faces.end(), face.begin(), face.end());
        return;
    }

    int longest_i = 0;
    double longest_length = 0;
    for (int i = 0; i < 3; i++) {
        if (side_lengths[i] > longest_length) {
            longest_length = side_lengths[i];
            longest_i = i;
        }
    }
    
	Eigen::Vector3f new_midpoint = GetMidpoint(vertices[face[longest_i % 3]], vertices[face[(longest_i + 1) % 3]]);
    vertices.push_back(new_midpoint);
    vertex_classes.push_back( ( vertices.size() % 2 == 0 ) ? vertex_classes[face[longest_i % 3]] : vertex_classes[face[(longest_i + 1) % 3]] );
    
    std::vector<uint32_t> first_sub_face(3);
	first_sub_face[0] = face[longest_i % 3];
	first_sub_face[1] = face[(longest_i + 2) % 3];
    first_sub_face[2] = vertices.size()-1;
    
	std::vector<uint32_t> second_sub_face(3);
	second_sub_face[0] = face[(longest_i + 1) % 3];
	second_sub_face[1] = face[(longest_i + 2) % 3];
    second_sub_face[2] = vertices.size()-1;
    
    SplitFace(voxel_grid, vertices, vertex_classes, first_sub_face, sub_faces);
    SplitFace(voxel_grid, vertices, vertex_classes, second_sub_face, sub_faces);
            
}

float MultiClassVoxelizer::AreaOfTriangle(const Eigen::Vector3f& vertex_1,
                                          const Eigen::Vector3f& vertex_2,
                                          const Eigen::Vector3f& vertex_3) {

    return (vertex_2 - vertex_1).cross(vertex_3 - vertex_1).norm() / 2.0;

}

Eigen::Vector3f MultiClassVoxelizer::GetMidpoint(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) {
    return (v1 + v2) / 2;
}
           
float MultiClassVoxelizer::EuclideanDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) {
    return std::sqrt(std::pow((v1[0] - v2[0]),2) + std::pow((v1[1] - v2[1]),2) + std::pow((v1[2] - v2[2]),2));
}
