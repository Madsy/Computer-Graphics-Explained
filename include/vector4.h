#ifndef VECTOR4_H_GUARD
#define VECTOR4_H_GUARD
#include <cmath>

#include "vector3.h"

template<class T>
struct Vector4 : public Vector3<T>
{
  T w;
  Vector4() : Vector3<T>(), w(T(1.0)){}
  Vector4(T a, T b, T c, T d = 1.0f) : Vector3<T>(a,b,c), w(d){}
  Vector4(const Vector3<T>& v, T d = 1.0f) : Vector3<T>(v), w(d){}
  bool operator<(const Vector4<T>& v) const
  {
	return this->z > v.z;
  }
};

#endif
