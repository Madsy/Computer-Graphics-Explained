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
