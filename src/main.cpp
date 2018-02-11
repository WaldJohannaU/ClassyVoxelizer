/*
 Classy Voxelizer
 
 BSD 2-Clause License
 Copyright (c) 2018, Dario Rethage
 See LICENSE at package root for full license
 */

#include <iostream>
#include <string>

#include "ClassyVoxelizer.h"

int main (int argc, char* argv[]) {
    if (argc < 5) {
        const std::string usage_message =
            "\nUsage:\n\n./classyvoxelizer <input> <output> <voxel_size> <class/color> <voxel_mesh_output>\n";
        std::cout << usage_message << std::endl;
        return 0;
    }
    ClassyVoxelizer classy_voxelizer(std::stod(argv[3]));
    classy_voxelizer.Process(std::string(argv[4]) == "color" ? VoxelType::color : VoxelType::label,
                             argv[1], argv[2], (argc >= 5) ? argv[5] : "");
    return 0;
}
