#pragma once
#include "volumeData.h"
#include "Eigen/Dense"
//ref https://en.wikipedia.org/wiki/Trilinear_interpolation
class Interpolator
{
public:
    virtual volumeData interpolate(Eigen::Vector3f &p, const voxel &voxel) = 0;
};
class NearestNeighbourInterpolator : public Interpolator
{
    inline volumeData interpolate(Eigen::Vector3f &p, const voxel &voxel)
    {
        //Here your NNInterpolator code
        volumeData result = voxel.c000;
        std::vector<volumeData> v_list = {voxel.c001, voxel.c010, voxel.c011, voxel.c100, voxel.c101, voxel.c110,voxel.c111};
        float dist = (p - voxel.c000.position).norm();
        for (const auto& v : v_list) {
            float dis = (v.position - p).norm();
            if (dis < dist) {
                dist = dis;
                result = v;
            }
        }
        return result;
    };
};
class TrilinearInterpolator : public Interpolator
{

    inline volumeData interpolate(Eigen::Vector3f &p, const voxel &voxel)
    {
        //Here your TrilinearInterpolator code
        float xd = abs(p.x() - voxel.c000.position.x()) / (voxel.c111.position.x() - voxel.c000.position.x());
        float yd = abs(p.y() - voxel.c000.position.y()) / (voxel.c111.position.y() - voxel.c000.position.y());
        float zd = abs(p.z() - voxel.c000.position.z()) / (voxel.c111.position.z() - voxel.c000.position.z());
        if(xd * yd *zd < 0) {
            std::cout<<"ERROR INTERPOLATE"<<std::endl;
        }
        volumeData c00(voxel.c000, voxel.c100, xd),
        c01(voxel.c001, voxel.c101, xd),
        c10(voxel.c010, voxel.c110, xd),
        c11(voxel.c011, voxel.c111, xd);
        volumeData c0(c00, c10, yd), c1(c01, c11, yd);
        return volumeData(c0, c1, zd);
    };
};
