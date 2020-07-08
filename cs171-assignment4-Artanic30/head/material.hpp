#pragma once
#include "Eigen/Dense"
#include "interaction.hpp"

class BRDF
{
public:
	BRDF()
	{
		isSpecular = false;
	}
	
	// Evaluate the BRDF
	// The information in @Interaction contains ray's direction, normal
	// and other information that you might need
	// 入射光和反射光的比值
	virtual Eigen::Vector3f eval(Interaction &_interact) = 0;
	
	// Sample a direction based on the BRDF
	// The sampled direction is stored in @Interaction
	// The PDF of this direction is returned
	virtual float sample(Interaction &_interact) = 0;
	
	// Mark if the BRDF is specular
	bool isSpecular;
};


class IdealDiffuse : public BRDF
{
public:
	
	Eigen::Vector3f eval(Interaction& _interact) override
	{
		if (_interact.inputDir.dot(_interact.normal) <= 0
            || _interact.outputDir.dot(_interact.normal) <= 0) {
            return {0,0,0};
		}
		//std::cout<<"check eval output: "<<_interact.surfaceColor<<std::endl
		//<<"result: "<<_interact.surfaceColor * (1 / M_PI);
        //std::cout<<"find diffuse match"<<std::endl;
		float tem = _interact.outputDir.dot(_interact.normal);
		tem = tem < 0 ? 0 : tem;
		tem = tem > 1 ? 1 : tem;
		//std::cout<<"eval: "<<std::endl<<_interact.surfaceColor * (1 / M_PI) * tem<<std::endl;
		return _interact.surfaceColor * (1 / M_PI) * tem;
	};

	float sample(Interaction& _interact) override
	{
        float r = 1;
        float phi = 2.0f * M_PI * rand() / double(RAND_MAX);
        float x = r * cosf(phi);
        float y = r * sinf(phi);
        // project to hemisphere
        float z = 1 - x * x - y * y;
        //std::cout<<"sample direction: "<<x<<" "<<y<<" "<<z<<std::endl;
        _interact.outputDir[0] = x;
        _interact.outputDir[1] = y;
        _interact.outputDir[2] = z;
        _interact.outputDir = _interact.outputDir.normalized();
        float pdf = (1 / M_PI);
        //std::cout<<"sample pdf: "<<pdf<<std::endl;
        return pdf;
    };
};

class IdealSpecular : public BRDF
{
public:

	IdealSpecular() {
		isSpecular = true;
	}

	Eigen::Vector3f eval(Interaction &_interact) override
	{
        if (_interact.inputDir.dot(_interact.normal) <= 0
        || _interact.outputDir.dot(_interact.normal) <= 0
        ) {
            //std::cout<<"data: "<<std::endl<<_interact.inputDir<<std::endl<<_interact.outputDir<<std::endl<<_interact.normal<<std::endl;
            //std::cout<<"out"<<std::endl;
            return {0,0,0};
        }
        //std::cout<<"find specular match"<<std::endl;
        float tem = _interact.outputDir.dot(_interact.normal);
        tem = tem < 0 ? 0 : tem;
        tem = tem > 1 ? 1 : tem;
        return _interact.surfaceColor * tem;
	};

	float sample(Interaction& _interact) override
	{
	    /*
        Eigen::Vector3f O_point {_interact.entryPoint.x(), _interact.entryPoint.y(), _interact.entryPoint.z()};
	    Eigen::Vector3f A_point {O_point.x() - _interact.inputDir.x()
                        , O_point.y() - _interact.inputDir.y()
                        , O_point.z() - _interact.inputDir.z()};
	    Eigen::Vector3f OB, AO, ON;
	    ON = _interact.normal;
	    AO = -A_point;
	    OB = AO + 2*(-AO.dot(ON))*ON;
        _interact.outputDir[0] = OB.x();
        _interact.outputDir[1] = OB.y();
        _interact.outputDir[2] = OB.z();
        std::cout<<"inside input"<<std::endl<<_interact.inputDir<<std::endl;
        std::cout<<"inside output"<<std::endl<<OB<<std::endl;
        //std::cout<<"compare result: "<<std::endl<<OB<<" and "<<-_interact.inputDir + abs(2*_interact.outputDir.dot(_interact.normal))*_interact.normal<<std::endl;
        if (AO.cross(ON).dot(_interact.outputDir) != 0) {
            //std::cout<<"input: "<<std::endl<<-AO<<std::endl;
            //std::cout<<"normal: "<<std::endl<<ON<<std::endl;
            //std::cout<<"output: "<<std::endl<<_interact.outputDir<<std::endl;
            //std::cout<<"Ao cross NO: "<<std::endl<<AO.cross(ON)<<std::endl;
            std::cout<<"Error in reflection"<<std::endl;
        }
        */
	    Eigen::Vector3f result;
	    result = _interact.inputDir + abs(-2*_interact.inputDir.dot(_interact.normal))*_interact.normal;
	    /*
	    _interact.outputDir[0] = _interact.inputDir.x();
        _interact.outputDir[1] = -_interact.inputDir.y();
        _interact.outputDir[2] = _interact.inputDir.z();
        */
        _interact.outputDir[0] = result.x();
        _interact.outputDir[1] = result.y();
        _interact.outputDir[2] = result.z();
        //std::cout<<"compare"<<std::endl<<result<<std::endl;
        //std::cout<<"right"<<std::endl<<_interact.outputDir<<std::endl;
		return 1;
	};
};