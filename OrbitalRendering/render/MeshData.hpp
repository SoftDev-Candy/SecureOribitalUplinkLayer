//
// Created by Candy on 4/30/2026.
//

#ifndef SOUL_MESHDATA_HPP
#define SOUL_MESHDATA_HPP

#include<vector>

//Struct to store mesh data
struct MeshData
{

std::vector<float>vertices; //X Y Z  | X Y Z
std::vector<unsigned int>indices;// 0 1 2 3

};

#endif //SOUL_MESHDATA_HPP
