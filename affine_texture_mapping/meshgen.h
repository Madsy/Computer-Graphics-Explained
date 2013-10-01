/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

#ifndef MESHGEN_GUARD_H
#define MESHGEN_GUARD_H
#include <linealg.h>

void makeMeshSphere(std::vector<Vector4f>& dst, float radius);
void makeMeshCircle(std::vector<Vector4f>& dst, float radius);
void makeMeshPlane(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& tcoordData,
		  float size);
void makeMeshCube(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& tcoordData,
		  float size);
#endif
