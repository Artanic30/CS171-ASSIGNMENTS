#pragma once
#include "volume.h"
#include "camera.hpp"
#include <iostream>
//https://github.com/yuki-koyama/tinycolormap
#include "tinycolormap.hpp"

class Classifier{
    public:
    virtual opticsData transfer(volumeData v_data,float dt,Camera* cam ,std::vector<Light *> lights,float grad_max_norm=1)=0;
};

class myClassifier:public Classifier{
    public:
    opticsData transfer(volumeData v_data,float dt,Camera* cam,std::vector<Light *> lights,float grad_max_norm=1){
        opticsData optics;
		// Write your own classifier, make use of input args(may not all)
		// should set transparency and color of optics
		Eigen::Vector3f dir_ray = (cam->m_Pos - v_data.position).normalized();
        tinycolormap::Color tem = tinycolormap::GetHeatColor(v_data.density);
		Eigen::Vector3f surface_color = {tem.r(), tem.b(), tem.g()};
		surface_color *= pow(v_data.density,2);
		// ambient
		optics.color += surface_color * 0.1;
		Eigen::Vector3f normal = (v_data.gradient).normalized();
		for (const auto& light : lights) {
            Eigen::Vector3f lightDir = (light->m_Pos - v_data.position).normalized();
            // diffuse
            float diff = std::max<float>(normal.dot(lightDir), 0.0);
            optics.color += diff * surface_color;

            // specular
            Eigen::Vector3f reflectDir = (-lightDir + abs(-2*-lightDir.dot(normal))*normal).normalized();
            float spec = pow(std::max<float>(dir_ray.dot(reflectDir), 0.0), 32);
            optics.color += surface_color * spec;
        }
		double tran = (exp(pow(-v_data.density, 2) - 0.36)) / (1 - 0.36);
		tran = pow(tran, -4*dt);
        optics.transparency = Eigen::Vector3f(tran,tran,tran) + v_data.gradient/50;
        return optics;
    }
};