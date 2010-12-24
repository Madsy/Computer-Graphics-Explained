#ifndef RASTERIZER_H_GUARD
#define RASTERIZER_H_GUARD
#include <linealg.h>

void DrawTriangle(
		  std::vector<Vector4f>& triangle,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  );

void TriangleSplit(std::vector<Vector4f>& triangle);
#endif
