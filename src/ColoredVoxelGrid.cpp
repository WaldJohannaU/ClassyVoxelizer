/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */
#include "ColoredVoxelGrid.h"

ColoredVoxelGrid::ColoredVoxelGrid(const Eigen::Vector3f& grid_min,
                                   const Eigen::Vector3f& grid_max,
                                   float voxel_size):
    VoxelGridInterface(grid_min, grid_max, voxel_size)  {
    _voxelgrid.resize(_num_voxels, Eigen::Vector3i(-1, -1, -1));
}

bool ColoredVoxelGrid::IsVoxelOccupied(uint32_t voxel_id) const {
	return (_voxelgrid[voxel_id][0] != -1 || _voxelgrid[voxel_id][1] != -1 || _voxelgrid[voxel_id][2] != -1) ? true : false;
}

void ColoredVoxelGrid::SetVoxelColor(const uint32_t voxel_id, const Eigen::Vector3i& color) {
    if (voxel_id < _num_voxels)
        _voxelgrid[voxel_id] = color;
}

Eigen::Vector3i ColoredVoxelGrid::GetVoxelColor(const uint32_t voxel_id) const {
    if (voxel_id < _num_voxels)
        return _voxelgrid[voxel_id];

    return Eigen::Vector3i(-1,-1,-1);
}

const std::vector<Eigen::Vector3i>& ColoredVoxelGrid::GetVoxelGrid() const {
    return _voxelgrid;
}

void ColoredVoxelGrid::SaveAsPLY(const std::string& filepath) const {
	unsigned int num_occupied_voxels = 0;
	for (const auto& voxel: _voxelgrid) {
		if (voxel != Eigen::Vector3i(-1, -1, -1))
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

					Eigen::Vector3i color = GetVoxelColor(voxel_id);
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

void ColoredVoxelGrid::SetVoxelColor(const Eigen::Vector3f& vertex, const Eigen::Vector3i& color) {
    const uint32_t voxel_id = GetEnclosingVoxelID(vertex);
    SetVoxelColor(voxel_id, color);
}

