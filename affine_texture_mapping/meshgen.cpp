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
    const int resolution = 64;
    float interp = 1.0f / (float)resolution;
    Vector4f middle(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4f v0(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4f v1(0.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i<=resolution; ++i){	
	float alpha0 = (float)(i+0) * interp * PI * 2.0f;
	float alpha1 = (float)(i+1) * interp * PI * 2.0f;
	
	v0.x = sin(alpha0);
	v0.y = cos(alpha0);
	v1.x = sin(alpha1);
	v1.y = cos(alpha1);

	dst.push_back(v0);
	dst.push_back(middle);
	dst.push_back(v1);
    }
}

void makeMeshPlane(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& tcoordData,
		  float size)
{
    const Vector4f v0( 1.0f * size,  1.0f * size,  0.0f);
    const Vector4f v1(-1.0f * size,  1.0f * size,  0.0f);
    const Vector4f v2( 1.0f * size, -1.0f * size,  0.0f);
    const Vector4f v3(-1.0f * size, -1.0f * size,  0.0f);

    const Vector4f t0( 1.0f,  1.0f,  0.0f);
    const Vector4f t1( 0.0f,  1.0f,  0.0f);
    const Vector4f t2( 1.0f,  0.0f,  0.0f);
    const Vector4f t3( 0.0f,  0.0f,  0.0f);


    vertexData.push_back(v0);
    vertexData.push_back(v1);
    vertexData.push_back(v2);
    vertexData.push_back(v2);
    vertexData.push_back(v1);
    vertexData.push_back(v3);

    tcoordData.push_back(t0);
    tcoordData.push_back(t1);
    tcoordData.push_back(t2);
    tcoordData.push_back(t2);
    tcoordData.push_back(t1);
    tcoordData.push_back(t3);
}

void makeMeshCube(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& tcoordData,
		  float size)
{
    size *= 0.5f;

    const Vector4f v0( 1.0f * size,  1.0f * size, -1.0f * size);
    const Vector4f v1(-1.0f * size,  1.0f * size, -1.0f * size);
    const Vector4f v2( 1.0f * size,  1.0f * size,  1.0f * size);
    const Vector4f v3(-1.0f * size,  1.0f * size,  1.0f * size);
    const Vector4f v4( 1.0f * size, -1.0f * size, -1.0f * size);
    const Vector4f v5(-1.0f * size, -1.0f * size, -1.0f * size);
    const Vector4f v6( 1.0f * size, -1.0f * size,  1.0f * size);
    const Vector4f v7(-1.0f * size, -1.0f * size,  1.0f * size);

    const Vector4f t0( 1.0f,  1.0f,  0.0f);
    const Vector4f t1( 0.0f,  1.0f,  0.0f);
    const Vector4f t2( 1.0f,  0.0f,  0.0f);
    const Vector4f t3( 0.0f,  0.0f,  0.0f);

    /* Top */
    vertexData.push_back(v0);
    vertexData.push_back(v1);
    vertexData.push_back(v2);
    vertexData.push_back(v2);
    vertexData.push_back(v1);
    vertexData.push_back(v3);
    /* Bottom */
    vertexData.push_back(v5);
    vertexData.push_back(v4);
    vertexData.push_back(v7);
    vertexData.push_back(v7);
    vertexData.push_back(v4);
    vertexData.push_back(v6);

    /* Right */
    vertexData.push_back(v0);
    vertexData.push_back(v2);
    vertexData.push_back(v4);
    vertexData.push_back(v4);
    vertexData.push_back(v2);
    vertexData.push_back(v6);

    /* Left */
    vertexData.push_back(v3);
    vertexData.push_back(v1);
    vertexData.push_back(v7);
    vertexData.push_back(v7);
    vertexData.push_back(v1);
    vertexData.push_back(v5);
    /* Back */
    vertexData.push_back(v0);
    vertexData.push_back(v1);
    vertexData.push_back(v4);
    vertexData.push_back(v4);
    vertexData.push_back(v1);
    vertexData.push_back(v5);
    /* Front */
    vertexData.push_back(v2);
    vertexData.push_back(v3);
    vertexData.push_back(v6);
    vertexData.push_back(v6);
    vertexData.push_back(v3);
    vertexData.push_back(v7);

    for(int i=0; i<6; ++i){
	tcoordData.push_back(t0);
	tcoordData.push_back(t1);
	tcoordData.push_back(t2);
	tcoordData.push_back(t2);
	tcoordData.push_back(t1);
	tcoordData.push_back(t3);
    }
}
