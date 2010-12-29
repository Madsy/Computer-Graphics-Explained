#ifndef CLIPPLANE_H_GUARD
#define CLIPPLANE_H_GUARD
#include <linealg.h>
void clip_triangle(std::vector<Vector4f>& vertexList, std::vector<Vector4f>& tcoordList, Vector4f plane);
int classifyTriangle(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
#endif
