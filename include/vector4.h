/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

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
  Vector4(const Vector4<T>& v) : Vector3<T>(v.x, v.y, v.z), w(v.w){}
  bool operator<(const Vector4<T>& v) const
  {
	return this->z > v.z;
  }
};

#endif
