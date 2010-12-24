#ifndef RASTERIZER_H_GUARD
#define RASTERIZER_H_GUARD
#include <linealg.h>

void DrawTriangle(
		  std::vector<Vector4i>& vertexData,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  );
void TriangleSplit(std::vector<Vector4i>& triangle);
#endif

