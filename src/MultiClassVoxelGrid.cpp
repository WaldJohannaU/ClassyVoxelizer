/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */
#include "MultiClassVoxelGrid.h"

MultiClassVoxelGrid::MultiClassVoxelGrid(const Eigen::Vector3f& grid_min,
                                         const Eigen::Vector3f& grid_max, float voxel_size):
    VoxelGridInterface(grid_min, grid_max, voxel_size) {
    _voxelgrid.resize(_num_voxels, 0);
}

bool MultiClassVoxelGrid::IsVoxelOccupied(const uint32_t voxel_id) const {
	return _voxelgrid[voxel_id] != 0;
}

void MultiClassVoxelGrid::SetVoxelClass(const uint32_t voxel_id, const uint8_t class_i) {
    if (voxel_id < _num_voxels)
        _voxelgrid[voxel_id] = class_i;
    else
    	std::cout << "Invalid voxel id" << std::endl;
}

int MultiClassVoxelGrid::GetVoxelClass(const uint32_t voxel_id) const {
    
    if(voxel_id < _num_voxels)
        return _voxelgrid[voxel_id];

    return -1;
}

const std::vector<uint8_t>& MultiClassVoxelGrid::GetVoxelGrid() const {
    return _voxelgrid;
}

void MultiClassVoxelGrid::SaveAsPLY(const std::string& filepath,
                                    const std::vector<Eigen::Vector3i>& class_color_mapping) const {

	unsigned int num_occupied_voxels = 0;
	for (const auto& voxel : _voxelgrid) {
		if (voxel != 0)
			num_occupied_voxels++;
	}

	std::vector<float> vertices(num_occupied_voxels * 3);
	std::vector<uint8_t> colors(num_occupied_voxels * 4);

	unsigned int raw_vertex_i = 0;
	unsigned int raw_color_i = 0;
	for (int i = 0; i < _voxels_per_dim[0]; i++) {
		for (int j = 0; j < _voxels_per_dim[1]; j++) {
			for (int k = 0; k < _voxels_per_dim[2]; k++) {
				
				uint32_t voxel_id = (unsigned int) _voxels_per_dim[0] * _voxels_per_dim[1] * k + _voxels_per_dim[0] * j + i;
				
				if (IsVoxelOccupied(voxel_id)) {

					uint8_t class_i = GetVoxelClass(voxel_id);
					Eigen::Vector3i color(class_color_mapping[class_i-1][0], class_color_mapping[class_i-1][1], class_color_mapping[class_i-1][2]);
					Eigen::Vector3f voxel_pos((i * _voxel_size) + _grid_min[0] + _voxel_size / 2, (j * _voxel_size) + _grid_min[1] + _voxel_size / 2, (k * _voxel_size) + _grid_min[2] + _voxel_size / 2);

					vertices[raw_vertex_i++] = voxel_pos[0];
					vertices[raw_vertex_i++] = voxel_pos[1];
					vertices[raw_vertex_i++] = voxel_pos[2];

					colors[raw_color_i++] = color[0];
					colors[raw_color_i++] = color[1];
					colors[raw_color_i++] = color[2];
					colors[raw_color_i++] = 255;

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
	out_file.write(ss, true);
	fb.close();
}

