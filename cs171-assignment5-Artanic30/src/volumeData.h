#pragma once
#include <vector>
#include <string>
#include "Eigen/Dense"
class volumeData{
    public:
    Eigen::Vector3f position;
    float density;
    Eigen::Vector3f gradient;
    inline volumeData(){};
    inline volumeData(float x,float y,float z, float den,Eigen::Vector3f grad = Eigen::Vector3f(0,0,0)){this->position = Eigen::Vector3f(x,y,z);this->density = den;this->gradient=grad;};
    inline volumeData(volumeData &a, volumeData &b, float xd){
        position = a.position * (1 - xd) + b.position * xd;
        density = a.density * (1 - xd) + b.density * xd;
        gradient = a.gradient * (1 - xd) + b.gradient * xd;
    };

    inline ~volumeData(){};
};

