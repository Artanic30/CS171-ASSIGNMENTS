#pragma once

#include <utility>


#include "Eigen/Dense"
#include "film.hpp"
#include "ray.hpp"

#define M_PIf 3.14159265358979323846f

class Camera {
public:
    Camera(const Eigen::Vector3f& pos, const Eigen::Vector3f& lookAt, const Eigen::Vector3f& up, float verticalFov, const Eigen::Vector2i& filmRes) : m_Film(filmRes)
	{
        m_Pos = pos;
        /*
		 * TODO: Creat axes for camera here, which concerns m_Forward, m_Right and m_Up variables
		 */
        // follow the instruction on article negative Z axis point at origin
        m_Forward = -(lookAt - pos).normalized();
        m_Right = up.cross(m_Forward).normalized();
        m_Up = m_Right.cross(m_Forward).normalized();
        FOV = verticalFov;
        m_Film = Film(filmRes);
    }

	// Generate camera ray
	// dx and dy is the pixel's coordinate
	Ray generateRay(int dx, int dy)
	{
        // todo: world space * lookAt = camera space
        // camera didn't rotate ignore lookAt for now
        double Px = (2 * ((dx + 0.5) / m_Film.m_Res.x()) - 1) * tan( FOV / 2 * M_PIf / 180) * m_Film.getAspectRatio();
        double Py = (1 - 2 * ((dy + 0.5) / m_Film.m_Res.y())) * tan( FOV / 2 * M_PIf / 180);
        Ray ray(m_Pos, Eigen::Vector3f(Px, -Py, -1));
        return ray;
    }

    void setPixel(int dx, int dy, Eigen::Vector3f value)
    {
        m_Film.pixelSamples[dy * m_Film.m_Res.x() + dx] = std::move(value);
    }
	
    Eigen::Vector3f m_Pos;
	
    Eigen::Vector3f m_Forward;
    Eigen::Vector3f m_Right;
    Eigen::Vector3f m_Up;
    float FOV;
	
    Film m_Film;
};
