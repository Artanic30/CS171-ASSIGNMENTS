#pragma once
#include <vector>
#include <string>
#include "Eigen/Dense"
#include <iostream>
#include <fstream>
#include "aabb.hpp"
#include "volumeData.h"
#include "voxel.h"
#include "ray.hpp"
class Volume{
private:
    std::vector<volumeData> raw_data;
    inline int _index_to_i(int,int,int);
public:
    Eigen::Vector3i size;
    Eigen::Vector3f size_physics;
    AABB bbox;
    double dx;
    float grad_maxnorm;
    Volume();
    ~Volume();
    Volume(std::string volumefile);
    bool getRayStartEnd(Ray& ray,float& t_start,float & t_end);
    void computeGradients();
    voxel getVoxel(Eigen::Vector3f position);
    volumeData& indexToData(Eigen::Vector3i index);

};