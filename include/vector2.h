/*
* Copyright (c) 2010, Mads Andreas Elvheim, mads@mechcore.net
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the organization nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Mads Andreas Elvheim ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Mads Andreas Elvheim BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
