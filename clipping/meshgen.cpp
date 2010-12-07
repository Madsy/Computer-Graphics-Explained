#include <vector>
#include <linealg.h>
#include "meshgen.h"

void makeMeshSphere(std::vector<Vector4f>& dst, float radius)
{

    const int resolution = 16;
    const float halfPI = PI * 0.5f;
    float interp = 1.0f / (float)resolution;
    Vector4f v0,v1,v2,v3;
    radius *= 0.5f;
    
    for(int i=0; i<=resolution; ++i){
        float theta0 = interp*(float)(i+0)*PI - halfPI;
        float theta1 = interp*(float)(i+1)*PI - halfPI;
        float z1 = std::sin(theta0);
        float z2 = std::sin(theta1);
        
        v0.z = z1 * radius;
        v1.z = z1 * radius;
        v2.z = z2 * radius;
        v3.z = z2 * radius;
        
        for(int j=0; j<=resolution; ++j){
            float phi0 = interp*(float)(j+0)*2.0f*PI;
            float phi1 = interp*(float)(j+1)*2.0f*PI;
            float x1 = std::cos(theta0)*std::cos(phi0);
			float x2 = std::cos(theta0)*std::cos(phi1);
            float y1 = std::cos(theta0)*std::sin(phi0);
            float y2 = std::cos(theta0)*std::sin(phi1);
			float x3 = std::cos(theta1)*std::cos(phi0);
			float x4 = std::cos(theta1)*std::cos(phi1);
            float y3 = std::cos(theta1)*std::sin(phi0);
            float y4 = std::cos(theta1)*std::sin(phi1);
            v0.x = x1 * radius;
            v0.y = y1 * radius;
            v1.x = x2 * radius;
            v1.y = y2 * radius;
            v2.x = x3 * radius;
            v2.y = y3 * radius;
            v3.x = x4 * radius;
            v3.y = y4 * radius;
            
            dst.push_back(v0); dst.push_back(v1); dst.push_back(v2);
            dst.push_back(v2); dst.push_back(v1); dst.push_back(v3);
        }
    }
}

void makeMeshCircle(std::vector<Vector4f>& dst, float radius)
{
    const int resolution = 16;
    float interp = 1.0f / (float)resolution;
    Vector4f middle(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4f v0(0.0f, 0.0f, 0.0f);
    Vector4f v1(0.0f, 0.0f, 0.0f);

    for(int y = 0; y<=16; ++y){	
	float y0 = (float)(y+0) * interp * PI * 2.0;
	float y1 = (float)(y+1) * interp * PI * 2.0;
        v0.y = cos(y0);
        v1.y = cos(y1);
	for(int x=0; x<=16; ++x){
	    float x0 = (float)(x+0) * interp * PI * 2.0;
	    float x1 = (float)(x+1) * interp * PI * 2.0;
	    v0.x = sin(x0);
	    v1.x = sin(x1);

	    dst.push_back(v1.unit() * radius * 0.5f);
	    dst.push_back(middle);
	    dst.push_back(v0.unit() * radius * 0.5f);
	}
    }
}
