#pragma once
#include "integrator.hpp"
#include "material.hpp"
#include <cmath>
//#include <omp.h>

class PathTracingIntegrator : public Integrator
{
public:
	PathTracingIntegrator(Scene* scene, Camera* camera)
		: Integrator(scene, camera)
	{
	}

	// main render loop
	void render() override
	{
		// Don't forget to call @Camera.setPixel() at the end
		std::cout<<"render...."<<std::endl;
        for (int dx = 0; dx < camera->m_Film.m_Res.x(); dx++)
        {
            std::cout<<"progress: "<<dx<<std::endl;
            for (int dy = 0; dy < camera->m_Film.m_Res.y(); dy++)
            {
                Ray ray = camera->generateRay(dx, dy);
                Interaction surfaceInteraction;
                if (scene->intersection(&ray, surfaceInteraction))
                {
                    Eigen::Vector3f res {0,0,0};
                    int repeat = 64;
                    for (int i = 0; i < repeat; ++i) {
                        res += radiance(&surfaceInteraction, &ray);
                    }
                    res /= repeat;
                    camera->setPixel(dx, dy, res);
                    for (auto light : scene->lights) {
                        float delta = 0;
                        for (int j = 0; j < 3; ++j) {
                            delta += abs(light->m_Pos[j] - surfaceInteraction.entryPoint[j]) * abs(light->m_Pos[j] - surfaceInteraction.entryPoint[j]);
                        }
                        if (delta < 2) {
                            camera->setPixel(dx, dy, light->m_Color);
                        }
                    }
                }
            }
        }
	}

	// radiance of a specific point
	Eigen::Vector3f radiance(Interaction * interaction, Ray * ray) override
	{
        Eigen::Vector3f result(0,0,0);
	    Ray my_ray(ray->m_Ori, ray->m_Dir);
	    // std::cout<<std::endl<<std::endl<<"beta begin"<<std::endl;
        bool specularBounce = false, check_result = false;
        int bounces;
        float beta = 1;
        //std::cout<<"new interaction"<<std::endl;
        for (bounces = 0; bounces < Max_bounce; ++bounces) {
            Interaction inter;
            //std::cout<<"interaction input: "<<std::endl<<interaction->inputDir<<std::endl;
            bool foundIntersection = scene->intersection(&my_ray, inter);
            inter.inputDir = my_ray.m_Dir;

            // 第一次反射一定会记录，或者有反射
            if (bounces == 0 || specularBounce) {
                // Add emitted light at path vertex or from the environment
                if (foundIntersection) {
                    result += beta * inter.surfaceColor;
                } else {
                    for (auto light : scene->lights) {
                        result += beta * light->m_Color;
                    }
                }
            }
            // 光线没有相交或超过最大次数
            if (!foundIntersection || bounces >= Max_bounce) {
                break;
            }


            Eigen::Vector3f light_pos, light_color;
            float light_pdf = 0;
            int repeat_light = 8;
            Interaction light_inter;

            for (int i = 0; i < repeat_light; ++i) {
                for (auto light : scene->lights) {
                    light_color = light->SampleSurfacePos(light_pos,light_pdf);
                    Eigen::Vector3f lightDir = (light_pos - inter.entryPoint).normalized();
                    Ray to_light(inter.entryPoint + lightDir * my_ray.m_fMin, lightDir);
                    if (!light->isHit(&to_light)) {
                        float cos = inter.normal.normalized().dot(lightDir);
                        result += beta * inter.surfaceColor / repeat_light * cos * 2;
                    } else {
                        beta += 0.1;
                    }
                }
            }

            BRDF* material = (BRDF*) inter.material;
            //std::cout<<inter.outputDir<<std::endl;
            float pdf = material->sample(inter);

            //std::cout<<inter.outputDir<<std::endl;
            Eigen::Vector3f f = material->eval(inter);

            beta *= pdf;
            //std::cout<<"beta1: "<<std::endl<<beta<<std::endl;
            if (pdf == 0.0f) {
                break;
            }
            //std::cout<<"beta1: "<<std::endl<<f <<std::endl;
            //beta = mul(beta, f * (abs(inter.inputDir.normalized().dot(inter.normal.normalized())) / pdf));
            //std::cout<<"beta2: "<<std::endl<<beta<<std::endl;
            specularBounce = material->isSpecular;
            my_ray.m_Dir = inter.outputDir;
            my_ray.m_Ori = inter.entryPoint + my_ray.m_Dir * my_ray.m_fMin;
        }
		return result/(bounces + 1);
	};

	static Eigen::Vector3f normalize(Eigen::Vector3f in) {
	    float factor = sqrt(in.x() * in.x() + in.y() * in.y() + in.z() * in.z());
        return in / factor;
	}

    static Eigen::Vector3f mul(const Eigen::Vector3f& a, const Eigen::Vector3f& b) {
        return {a.x() * b.x(), a.y() * b.y(), a.z() * b.z()};
    }

	int Max_bounce = 5;
};	



