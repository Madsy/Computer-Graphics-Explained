/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

#ifndef CLIPPLANE_H_GUARD
#define CLIPPLANE_H_GUARD
#include <linealg.h>
void clip_triangle(std::vector<Vector4f>& vertexList, std::vector<Vector4f>& tcoordList, Vector4f plane);
int classifyTriangle(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
#endif
