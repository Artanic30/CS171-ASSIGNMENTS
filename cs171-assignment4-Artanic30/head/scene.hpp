#pragma once

#include <utility>
#include <vector>
#include "light.hpp"
#include "shape.hpp"
#include "material.hpp"
class Scene
{
public:
	std::vector<Shape *> shapes;
	std::vector<Light *> lights;
	Scene()
	{
	}
	
	void addLight(Light * light)
	{
		lights.push_back(light);
	}
	
	void addShape(Shape * shape)
	{
		shapes.push_back(shape);
	}
	
	int getShapeCount() const
	{
		return shapes.size();
	}

	bool intersection(Ray * ray, Interaction& interaction)
	{
		Interaction surfaceInteraction;
		// ignore the last shape light
        for (int i = 0;i < shapes.size();i++)
		{
			Interaction curInteraction;
			if (shapes[i]->m_BoundingBox.rayIntersection(*ray, curInteraction.entryDist, curInteraction.exitDist))
			{
				curInteraction.entryPoint = ray->getPoint(curInteraction.entryDist);
				if (shapes[i]->rayIntersection(curInteraction, *ray))
				{
					if (surfaceInteraction.entryDist == -1 || curInteraction.entryDist < surfaceInteraction.entryDist)
					{
						surfaceInteraction = curInteraction;
						surfaceInteraction.material = shapes[i]->material;
					}
				}
			}
		}
		surfaceInteraction.lightId = -1;
		for(int i = 0;i<lights.size();i++){
			if(lights[i]->isHit(ray)){
				surfaceInteraction.lightId = i;
			}
		}
		interaction = surfaceInteraction;
        return surfaceInteraction.entryDist != -1 && surfaceInteraction.entryDist >= ray->m_fMin &&
               surfaceInteraction.entryDist <= ray->m_fMax;
    }

	bool intersection(Ray * ray)
	{
		Interaction surfaceInteraction;
		for (auto & shape : shapes)
		{
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

        return surfaceInteraction.entryDist != -1 && surfaceInteraction.entryDist >= ray->m_fMin &&
               surfaceInteraction.entryDist <= ray->m_fMax;
    }
};
