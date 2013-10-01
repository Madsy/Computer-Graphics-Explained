/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

#ifndef MESHDATA_H_GUARD
#define MESHDATA_H_GUARD
#include <vector>
#include <linealg.h>

template<class T>
struct Vertex
{
    Vector4<T> point;
    Vector2<T> tcoord;
    /* Arrays of vertex attributes */
    std::vector<T>    attrib1D;
    std::vector< Vector2<T> > attrib2D;
    std::vector< Vector2<T> > attrib3D;
    std::vector< Vector2<T> > attrib4D;
};

typedef Vertex<float> Vertexf;
typedef Vertex<int> Vertexi;
typedef Vertexf Trianglef[3];
typedef Vertexi Trianglei[3];

enum Attrib1DType
{
    
};
enum Attrib2DType
{
    TEXCOORD=0
};
enum Attrib3DType
{
    COLOR_RGB=0,
    NORMAL3
};
enum Attrib4DType
{
    COLOR_RGBA=0,
    NORMAL4
};

#endif
