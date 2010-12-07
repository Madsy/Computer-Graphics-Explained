#ifndef VECTOR3_H_GUARD
#define VECTOR3_H_GUARD
#include <cmath>


template<class T>
struct Vector3
{
  T x,y,z;

  Vector3() : x(T(0.0f)), y(T(0.0f)), z(T(0.0f)){}
  Vector3(T a, T b, T c) : x(a), y(b), z(c){}
  
  Vector3<T> operator+(const Vector3<T>& v) const
  {
    return Vector3<T>(x + v.x, y + v.y, z + v.z);
  }
  Vector3<T> operator-(const Vector3<T>& v) const
  {
    return Vector3<T>(x - v.x, y - v.y, z - v.z);
  }
  
  Vector3<T> operator+(const T& v) const
  {
    return Vector3<T>(x+v, y+v, z+v);
  }
  Vector3<T> operator-(const T& v) const
  {
    return Vector3<T>(x-v, y-v, z-v);
  }
  Vector3<T> operator*(const T& v) const
  {
    return Vector3<T>(x*v, y*v, z*v);
  }
  Vector3<T> operator/(const T& v) const
  {
    return Vector3<T>(x/v, y/v, z/v);
  }

  Vector3<T>& operator+=(const Vector3<T>& v)
  {
    *this = *this + v;
    return *this;
  }
  Vector3<T>& operator-=(const Vector3<T>& v)
  {
    *this = *this - v;
    return *this;
  }

  Vector3<T>& operator+=(const T& v)
  {
    *this = *this + v;
    return *this;
  }
  Vector3<T>& operator-=(const T& v)
  {
    *this = *this - v;
    return *this;
  }
  Vector3<T>& operator*=(const T& v)
  {
    *this = *this * v;
    return *this;
  }
  Vector3<T>& operator/=(const T& v)
  {
    *this = *this / v;
    return *this;
  }

  bool operator<(const Vector3<T> v)
  {
	/* Sorry, it's just a hack so we don't have to create a functor to std::sort */
	return z > v.z;
  }

  T length() const
  {
    return std::sqrt(x*x + y*y + z*z);
  }
  Vector3<T> unit() const
  {
    float len = length();
    if(std::abs(len) < 1e-8f)
      return Vector3<T>(0,0,0);
    return *this / len;
  }
  void normalize()
  {
    *this = unit();
  }
};

template<class T>
inline T dot(const Vector3<T>& v1, const Vector3<T>& v2)
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<class T>
inline Vector3<T> cross(const Vector3<T>& v1, const Vector3<T>& v2)
{
  return Vector3<T>(v1.y*v2.z - v1.z*v2.y,
		    v1.z*v2.x - v1.x*v2.z,
		    v1.x*v2.y - v1.y*v2.x
		    );
}

#endif
