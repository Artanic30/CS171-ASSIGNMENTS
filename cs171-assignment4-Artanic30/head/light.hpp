#pragma once
#include <Eigen/Dense>
#include <utility>
#include <cmath>
#include <random>
#include "triangleMesh.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "interaction.hpp"


class Light
{
public:
	Light(Eigen::Vector3f pos, Eigen::Vector3f color)
		: m_Pos(std::move(pos)),
		m_Color(std::move(color))
	{
	}
	virtual ~Light() = default;

	// Sample a point on the light's surface, if light is not delta light
	// Set PDF and the surface position
	// Return color of light
	virtual Eigen::Vector3f SampleSurfacePos(Eigen::Vector3f& sampled_lightPos, float& pdf)=0;
	// Determine if light is hit, if light is not delta light
	virtual bool isHit(Ray* ray) = 0;
	
	Eigen::Vector3f m_Pos;
	Eigen::Vector3f m_Color;
};

class AreaLight:public Light
{
public:
	AreaLight(Eigen::Vector3f pos, Eigen::Vector3f color) : Light(pos, color)
	{
	}

	void add_shapes(std::vector<Shape *> *s) {
	    shapes = *s;
	}

	Eigen::Vector3f SampleSurfacePos(Eigen::Vector3f& sampled_lightPos, float & pdf) override {
        double range = 1;
	    pdf = 1;
        sampled_lightPos[0] = m_Pos.x() + (((rand() / double(RAND_MAX))*2 - 1)*range);
        sampled_lightPos[1] = m_Pos.y();
        sampled_lightPos[2] = m_Pos.z() + (((rand() / double(RAND_MAX))*2 - 1)*range);
        //std::cout<<"light position: "<<m_Pos<<std::endl;
		//std::cout<<"sample light position: "<<sampled_lightPos<<std::endl;
		return m_Color;
	};
	
	bool isHit(Ray * ray) override {
        Interaction surfaceInteraction;
        for (auto & shape : shapes)
        {
            if (shape->shape_type==0) {
                Interaction curInteraction;
                if (shape->m_BoundingBox.rayIntersection(*ray, curInteraction.entryDist, curInteraction.exitDist))
                {
                    if (shape->rayIntersection(curInteraction, *ray))
                    {
                        if (surfaceInteraction.entryDist == -1 || curInteraction.entryDist < surfaceInteraction.entryDist)
                        {
                            surfaceInteraction = curInteraction;
                        }
                    }
                }
            }
        }

        return surfaceInteraction.entryDist != -1 && surfaceInteraction.entryDist >= ray->m_fMin &&
               surfaceInteraction.entryDist <= ray->m_fMax;
	};

private:
    std::vector<Shape *> shapes;
};

