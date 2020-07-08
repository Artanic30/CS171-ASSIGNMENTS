#include "compositor.h"
#include <iostream>

Compositor::Compositor(){};
Compositor::~Compositor(){};
void Compositor::compositeFrontToBack(Eigen::Vector3f& color_dst,Eigen::Vector3f& alpha_dst,const Eigen::Vector3f& color_src,const Eigen::Vector3f& alpha_src){
    //std::cout<<"alpha_src: \n"<<alpha_src<<std::endl;
    //std::cout<<"color_src: \n"<<color_src<<std::endl;
    color_dst = color_dst + (Eigen::Vector3f::Ones() - alpha_dst).cwiseProduct(color_src);
    alpha_dst = alpha_dst + (Eigen::Vector3f::Ones() - alpha_dst).cwiseProduct(alpha_src);
    //std::cout<<"color_dst: \n"<<color_dst<<std::endl;
    //std::cout<<"alpha_dst: \n"<<alpha_dst<<std::endl<<std::endl;
};
void Compositor::compositeBackToFront(Eigen::Vector3f& color_dst,const Eigen::Vector3f& color_src,const Eigen::Vector3f& alpha_src){
    color_dst = (Eigen::Vector3f::Ones() - alpha_src).cwiseProduct(color_dst) + color_src;
};

