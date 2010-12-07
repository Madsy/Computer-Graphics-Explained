#ifndef CLIPPLANE_H_GUARD
#define CLIPPLANE_H_GUARD
void clip_triangle(std::vector<Vector4f>& vertexList, Vector4f plane);
int classifyTriangle(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
#endif
