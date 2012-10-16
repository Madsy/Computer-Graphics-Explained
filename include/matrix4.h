#ifndef MATRIX4_H_GUARD
#define MATRIX4_H_GUARD
#include <algorithm>
#include "vector4.h"

template<class T> struct Matrix4
{
  T m[16];
  Matrix4()
  {
    std::fill(m, m+16, T(0));
  }
  Matrix4(const Vector4<T>& c1,
	  const Vector4<T>& c2,
	  const Vector4<T>& c3,
	  const Vector4<T>& c4)
  {
    m[ 0] = c1.x; m[ 1] = c1.y; m[ 2] = c1.z; m[ 3] = c1.w;
    m[ 4] = c2.x; m[ 5] = c2.y; m[ 6] = c2.z; m[ 7] = c2.w;
    m[ 8] = c3.x; m[ 9] = c3.y; m[10] = c3.z; m[11] = c3.w;
    m[12] = c4.x; m[13] = c4.y; m[14] = c4.z; m[15] = c4.w;
  }

  T operator[](size_t index) const
  {
    return m[index];
  }

  T& operator[](size_t index)
  {
      return m[index];
  }

  void identity()
  {
    std::fill(m, m+16, T(0));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
  }

  Matrix4<T> operator*(const Matrix4<T>& mat) const
  {
    Matrix4<T> result;
    for(int i=0; i<4; ++i){
      for(int j=0; j<4; ++j){
		for(int k=0; k<4; ++k){
			result[j+i*4] += m[k + i*4]*mat[j + k*4];
		}
      }
    }
    return result;
  }
};

#endif
