/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include "MultiClassVoxelizer.h"

void MultiClassVoxelizer::Voxelize(MultiClassVoxelGrid& voxel_grid,
                                   std::vector<Eigen::Vector3f>& vertices,
                                                  std::vector<uint32_t>& faces,
                                                  std::vector<uint16_t>& vertex_classes) {


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
}

void MultiClassVoxelizer::SplitFace(MultiClassVoxelGrid& voxel_grid,
                                    std::vector<Eigen::Vector3f>& vertices,
                                    std::vector<uint16_t>& vertex_classes,
                                    std::vector<uint32_t>& face,
                                    std::vector<uint32_t>& sub_faces) {
    std::vector<uint32_t> first_sub_face(3);
    std::vector<uint32_t> second_sub_face(3);

    const int longest_i = SplitBaseFace(voxel_grid, vertices, face, sub_faces,
                                        first_sub_face, second_sub_face);
    if (longest_i == -1)
        return;
    vertex_classes.push_back((vertices.size() % 2 == 0 ) ?
                              vertex_classes[face[longest_i % 3]] :
                              vertex_classes[face[(longest_i + 1) % 3]]);
    
    SplitFace(voxel_grid, vertices, vertex_classes, first_sub_face, sub_faces);
    SplitFace(voxel_grid, vertices, vertex_classes, second_sub_face, sub_faces);
}
    
