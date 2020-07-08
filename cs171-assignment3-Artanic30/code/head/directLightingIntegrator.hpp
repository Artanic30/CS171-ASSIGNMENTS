#pragma once
#include "integrator.hpp"

class DirectLightingIntegrator : public Integrator
{
public:
	// blinn phong parameters
	float shininess = 16.0f;
	float lightPower = 15.0f;
	Eigen::Vector3f ambientColor = Eigen::Vector3f(0.1, 0.1, 0.1);
	Eigen::Vector3f specColor = Eigen::Vector3f(1.0, 1.0, 1.0);
	
	DirectLightingIntegrator(Scene* scene, Camera* camera)
		: Integrator(scene, camera)
	{
	}

	// main render loop
	void render() override
	{
		for (int dx = 0; dx < camera->m_Film.m_Res.x(); dx++)
		{
		    std::cout<<"progress: "<<dx<<std::endl;
			for (int dy = 0; dy < camera->m_Film.m_Res.y(); dy++)
			{
				Ray ray = camera->generateRay(dx, dy);
				Interaction surfaceInteraction;
				if (scene->intersection(&ray, surfaceInteraction))
				{
					camera->setPixel(dx, dy, radiance(&surfaceInteraction, &ray));
				}
			}
		}
	}

	// radiance of a specific point
	Eigen::Vector3f radiance(Interaction* interaction, Ray * ray) override
	{
        Eigen::Vector3f lightDir = (scene->light.m_Pos - interaction->entryPoint).normalized();
		Ray ray_to_light(interaction->entryPoint + lightDir * ray->m_fMin, lightDir);

		if (scene->intersection_withot_wall(&ray_to_light)) {
            return Eigen::Vector3f ((ambientColor).x() * interaction->surfaceColor.x(),
                    (ambientColor).y() * interaction->surfaceColor.y(),
                    (ambientColor).z() * interaction->surfaceColor.z());
		}
        // 漫反射光照
        // normalize片段法向量
        Eigen::Vector3f norm = interaction->normal.normalized();
        // 计算光照方向单位向量：光源位置 - 片段位置
        float diff = (norm).dot(lightDir) > 0.0f ? (norm).dot(lightDir) : 0.0f;
        // 漫反射光照 = 上面的漫反射强度值与光照颜色乘积
        Eigen::Vector3f diffuse = diff * 0.01f * scene->light.m_Color;
        Eigen::Vector3f result((diffuse + ambientColor + 0.8*specColor).x() * interaction->surfaceColor.x(),
                (diffuse + ambientColor + 0.8*specColor).y() * interaction->surfaceColor.y(),
                (diffuse + ambientColor + 0.8*specColor).z() * interaction->surfaceColor.z());
        return interaction->surfaceColor;
	}
};