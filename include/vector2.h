/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

#ifndef VECTOR2_H_GUARD
#define VECTOR2_H_GUARD
#include <cmath>

template<class T> struct Vector2
{
  T x, y;
  Vector2() : x(T(0.0f)), y(T(0.0f)){}
  Vector2(T a, T b) : x(a), y(b){}

  Vector2<T> operator+(const Vector2<T>& v) const
  {
    return Vector2<T>(x+v.x, y+v.y);
  }
  Vector2<T> operator-(const Vector2<T>& v) const
  {
    return Vector2<T>(x-v.x, y-v.y);
  }
  Vector2<T> operator+(T s) const
  {
    return Vector2<T>(x+s, y+s);
  }
  Vector2<T> operator-(T s) const
  {
    return Vector2<T>(x-s, y-s);
  }
  Vector2<T> operator*(T s) const
  {
    return Vector2<T>(x*s, y*s);
  }
  Vector2<T> operator/(T s) const
  {
    return Vector2<T>(x/s, y/s);
  }
	
  Vector2<T>& operator+=(const Vector2<T>& v)
  {
    *this = *this + v;
    return *this;
  }
  Vector2<T>& operator-=(const Vector2<T>& v)
  {
    *this = *this - v;
    return *this;
  }
  Vector2<T>& operator+=(T s)
  {
    *this = *this + s;
    return *this;
  }
  Vector2<T>& operator-=(T s)
  {
    *this = *this - s;
    return *this;
  }
  Vector2<T>& operator*=(T s)
  {
    *this = *this * s;
    return *this;
  }
  Vector2<T>& operator/=(T s)
  {
    *this = *this / s;
    return *this;
  }

  T length() const
  {
    return std::sqrt(x*x + y*y);
  }	
  Vector2<T> unit() const
  {
    float mag = length();
    if(std::abs(mag) < T(0.000001f))
      return Vector2<T>(T(0.0f), T(0.0f));
    return *this / mag;
  }
  void normalize()
  {
    *this = unit();
  }

  bool operator<(const Vector2<T>& v) const
  {
    T len1 = length();
    T len2 = v.length();
		
    return len1 < len2;
  }

  bool operator>(const Vector2<T>& v) const
  {
    T len1 = length();
    T len2 = v.length();
		
    return len1 > len2;
  }

};

template<class T>
T dot(const Vector2<T>& v1, const Vector2<T>& v2)
{
  return v1.x*v2.x + v1.y*v2.y;
}
#endif
