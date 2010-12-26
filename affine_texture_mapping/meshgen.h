#ifndef MESHGEN_GUARD_H
#define MESHGEN_GUARD_H
#include <linealg.h>

void makeMeshSphere(std::vector<Vector4f>& dst, float radius);
void makeMeshCircle(std::vector<Vector4f>& dst, float radius);
void makeMeshCube(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& tcoordData,
		  float size);
#endif
