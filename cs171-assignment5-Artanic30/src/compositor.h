#pragma once
#include "Eigen/Dense"
class Compositor{
    public:
    Compositor();
    ~Compositor();
    static void compositeFrontToBack(Eigen::Vector3f& color_dst,Eigen::Vector3f& alpha_dst,const Eigen::Vector3f& color_src,const Eigen::Vector3f& alpha_src);
    static void compositeBackToFront(Eigen::Vector3f& color_dst,const Eigen::Vector3f& color_src,const Eigen::Vector3f& alpha_src);
};