#ifndef LINEALG_H_GUARD
#define LINEALG_H_GUARD
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix4.h"

const float PI = 3.1415926535897932384626433832f;

template<class T>
Vector3<T> operator*(const Matrix4<T>& mat, const Vector3<T>& v)
{
  return Vector3<T>(
			v.x*mat[ 0] + v.y*mat[ 1] + v.z*mat[ 2] + mat[ 3],
		    v.x*mat[ 4] + v.y*mat[ 5] + v.z*mat[ 6] + mat[ 7],
		    v.x*mat[ 8] + v.y*mat[ 9] + v.z*mat[10] + mat[11]
		    );
}

template<class T>
Vector4<T> operator*(const Matrix4<T>& mat, const Vector4<T>& v)
{
  return Vector4<T>(
			v.x*mat[ 0] + v.y*mat[ 1] + v.z*mat[ 2] + v.w*mat[ 3],
		    v.x*mat[ 4] + v.y*mat[ 5] + v.z*mat[ 6] + v.w*mat[ 7],
		    v.x*mat[ 8] + v.y*mat[ 9] + v.z*mat[10] + v.w*mat[11],
		    v.x*mat[12] + v.y*mat[13] + v.z*mat[14] + v.w*mat[15]
		    );
}

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

typedef Vector4<int> Vector4i;
typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;

typedef Matrix4<int> Matrix4i;
typedef Matrix4<float> Matrix4f;
typedef Matrix4<double> Matrix4d;

/* Misc functions */

inline float degtorad(float deg)
{
    return deg/180.0f * PI;
}

inline float radtodeg(float theta)
{
    return theta/PI * 180.0f;
}

template<class T> T distance(const Vector3<T>& v1, const Vector3<T>& v2)
{
    Vector3<T> tmp = v2-v1;
    return tmp.length();
}

template<class T> T distance(const Vector4<T>& v1, const Vector4<T>& v2)
{
    Vector4<T> tmp = v2-v1;
    return tmp.length();
}

/* Linear interpolation */

Vector2f mix(float t, const Vector2f& v1, const Vector2f& v2)
{
	return v1*(t-1.0f) + v2*t;
}

Vector3f mix(float t, const Vector3f& v1, const Vector3f& v2)
{
	return v1*(t-1.0f) + v2*t;
}

Vector4f mix(float t, const Vector4f& v1, const Vector4f& v2)
{
	return v1*(t-1.0f) + v2*t;
}

/***************************************
 * Transformations for the three axes  *
 ***************************************/

inline Matrix4f rotateX(float deg)
{
    float s = std::sin(degtorad(deg));
    float c = std::cos(degtorad(deg));
    
    return Matrix4f(Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
                    Vector4f(0.0f,    c,   -s, 0.0f),
                    Vector4f(0.0f,    s,    c, 0.0f),
                    Vector4f(0.0f, 0.0f, 0.0f, 1.0f)
                    );
}

inline Matrix4f rotateY(float deg)
{
    float s = std::sin(degtorad(deg));
    float c = std::cos(degtorad(deg));
    
    return Matrix4f(Vector4f(   c, 0.0f,    s, 0.0f),
                    Vector4f(0.0f, 1.0f, 0.0f, 0.0f),
                    Vector4f(  -s, 0.0f,    c, 0.0f),
                    Vector4f(0.0f, 0.0f, 0.0f, 1.0f)
                    );
}

inline Matrix4f rotateZ(float deg)
{
    float s = std::sin(degtorad(deg));
    float c = std::cos(degtorad(deg));
    
    return Matrix4f(Vector4f(   c,   -s, 0.0f, 0.0f),
                    Vector4f(   s,    c, 0.0f, 0.0f),
                    Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
                    Vector4f(0.0f, 0.0f, 0.0f, 1.0f)
                    );
}

inline Matrix4f translate(const Vector4f& offset)
{
    return Matrix4f(
                    Vector4f(1.0f, 0.0f, 0.0f, offset.x),
                    Vector4f(0.0f, 1.0f, 0.0f, offset.y),
                    Vector4f(0.0f, 0.0f, 1.0f, offset.z),
                    Vector4f(0.0f, 0.0f, 0.0f,     1.0f)
                    );
}
/***************************************
 * Sets up our perspective clip matrix *
 ***************************************/
inline Matrix4f perspective(float fov, float aspect, float near, float far)
{
    /* Restrict fov to 179 degrees, for numerical stability */
    if(fov >= 180.0f)
        fov = 179.0f;
    
    float y = 1.0f / std::tan(degtorad(fov) * 0.5f);
    float x = y/aspect;
    float z1 = (far+near)/(near-far);
    float z2 = (2.0f*far*near)/(near-far);
    Matrix4f m(Vector4f(x, 0,  0,  0),
               Vector4f(0, y,  0,  0),
               Vector4f(0, 0, z1, z2),
               Vector4f(0, 0,  -1,  0)
               );
    return m;
}

/*********************************************************************
 * Projects our 4D clip coordinates down to 2D viewport coordinates. *
 * z and w are preserved for later use. x and y are now in pixel units*
 *********************************************************************/
inline Vector4f project(const Vector4f& v, float width, float height)
{
    Vector4f proj;
    float centerX = width*0.5f;
    float centerY = height*0.5f;

    proj.x = v.x*centerX  + centerX;
    proj.y = v.y*centerY + centerY;
    proj.z = v.z*0.5f + 0.5f;
    proj.w = v.w;

    return proj;
}

#endif
