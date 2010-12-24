#ifndef MESHGEN_GUARD_H
#define MESHGEN_GUARD_H
#include <linealg.h>

void makeMeshSphere(std::vector<Vector4f>& dst, float radius);
void makeMeshCircle(std::vector<Vector4f>& dst, float radius);
#endif
