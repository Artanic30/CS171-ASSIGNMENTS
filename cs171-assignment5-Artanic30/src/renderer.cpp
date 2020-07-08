#include "renderer.h"
void Renderer::setCamera(Camera * cam){this->camera=cam;};
void Renderer::addLight(Light * light){this->lights.push_back(light);};
void Renderer::setVolume(Volume * vol){this->volume=vol;};
void Renderer::setClassifier(Classifier * classifier){this->classifier=classifier;};
void Renderer::setInterpolator(Interpolator * interpolator){this->interpolator=interpolator;};
void Renderer::renderFrontToBack(std::string imgPath){
#pragma omp parallel for
float min = 10, max = 0;
    for (int i = 0; i < camera->m_Film.m_Res.x() * camera->m_Film.m_Res.y(); i++) {
        int dy = i / camera->m_Film.m_Res.x();
        int dx = i - dy * camera->m_Film.m_Res.x();
        Eigen::Vector3f color(0,0,0);
        Eigen::Vector3f alpha(0,0,0);
        Ray ray = camera->generateRay((float)dx , (float)dy );
        float t_start; float t_end;
        //Intersection calculate
        if(this->volume->getRayStartEnd(ray,t_start,t_end)){
			// Here your render code
			float current_dis = t_start;
			float step_size = 0.05;
			//std::cout<<"start of a new round: "<<std::endl;
            while (current_dis < t_end) {
                Eigen::Vector3f sample_pos = ray.m_Ori + ray.m_Dir * current_dis;
                volumeData data = interpolator->interpolate(sample_pos, volume->getVoxel(sample_pos));
                opticsData opt_data = classifier->transfer(data, step_size, camera, lights);
                Compositor::compositeFrontToBack(color, alpha, opt_data.getColor(), opt_data.getOpacity());

                // shadow
                Eigen::Vector3f frag_pos = ray.m_Ori + ray.m_Dir * t_start;
                float total_density = 0;
                float shadow_current = 0;
                float shadow_t_start; float shadow_t_end;
                float shadow_step_size = 0.5;
                for (auto light : lights) {
                    Ray to_light(frag_pos, light->m_Pos - frag_pos);
                    if(this->volume->getRayStartEnd(to_light,shadow_t_start,shadow_t_end)){
                        shadow_current = shadow_t_start;
                        while (shadow_current < shadow_t_end) {
                            Eigen::Vector3f shadow_sample_pos = ray.m_Ori + ray.m_Dir * current_dis;
                            volumeData shadow_data = interpolator->interpolate(shadow_sample_pos, volume->getVoxel(shadow_sample_pos));
                            total_density += shadow_data.density;
                            shadow_current += shadow_step_size;
                        }
                    }
                }
                total_density = exp(-total_density/100);
                total_density = std::min<float>(total_density, 1);
                total_density = std::max<float>(total_density, 0);
                min = min > total_density ? total_density : min;
                max = max < total_density ? total_density : max;
                //std::cout<<(1- total_density)<<std::endl;
                color *= total_density;

                if (alpha == Eigen::Vector3f::Ones()) {
                    break;
                }
                current_dis += step_size;
            }

        }
        camera->setPixel(dx, dy, color);
    }
    std::cout<<max<<std::endl;
    std::cout<<min<<std::endl;
    camera->m_Film.write(imgPath);
}
void Renderer::renderBackToFront(std::string imgPath){
//#pragma omp parallel for
    for (int i = 0; i < camera->m_Film.m_Res.x() * camera->m_Film.m_Res.y(); i++) {
        int dy = i / camera->m_Film.m_Res.x();
        int dx = i - dy * camera->m_Film.m_Res.x();
        Eigen::Vector3f color(0,0,0);
        Ray ray = camera->generateRay((float)dx , (float)dy );
        float t_start; float t_end;
        //Intersection calculate
        if(this->volume->getRayStartEnd(ray,t_start,t_end)){
            float current_dis = t_end;
            float step_size = 0.05;
            while (current_dis > t_start) {
                Eigen::Vector3f sample_pos = ray.m_Ori + ray.m_Dir * current_dis;
                volumeData data = interpolator->interpolate(sample_pos, volume->getVoxel(sample_pos));
                opticsData opt_data = classifier->transfer(data, step_size, camera, lights);
                Compositor::compositeBackToFront(color, opt_data.getColor(), opt_data.getOpacity());
                current_dis -= step_size;

                // shadow
                Eigen::Vector3f frag_pos = ray.m_Ori + ray.m_Dir * t_start;
                float total_density = 0;
                float shadow_current = 0;
                float shadow_t_start; float shadow_t_end;
                float shadow_step_size = 0.5;
                for (auto light : lights) {
                    Ray to_light(frag_pos, light->m_Pos - frag_pos);
                    if(this->volume->getRayStartEnd(to_light,shadow_t_start,shadow_t_end)){
                        shadow_current = shadow_t_start;
                        while (shadow_current < shadow_t_end) {
                            Eigen::Vector3f shadow_sample_pos = ray.m_Ori + ray.m_Dir * current_dis;
                            volumeData shadow_data = interpolator->interpolate(shadow_sample_pos, volume->getVoxel(shadow_sample_pos));
                            total_density += shadow_data.density;
                            shadow_current += shadow_step_size;
                        }
                    }
                }
                total_density = exp(-total_density/100);
                total_density = std::min<float>(total_density, 1);
                total_density = std::max<float>(total_density, 0);
                //std::cout<<(1- total_density)<<std::endl;
                color *= total_density;
            }
        }
        camera->setPixel(dx, dy, color);
    }
    camera->m_Film.write(imgPath);
};