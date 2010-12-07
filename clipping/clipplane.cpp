#include <vector>
#include <algorithm>
#include <linealg.h>
#include "clipplane.h"

static Vector4f computeClippedPoint(const Vector4f& v1, const Vector4f& v2, float c1, float c2, float w)
{
    /*
    0.9 1.0 1.9
    
    */
    Vector4f p;
    float t = (w - c1) / (c2 - c1);
    p = mix(t, v1, v2);
}

/* true if inside */
int classifyTriangle(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
{
    int bx1 = (-v1.w <= v1.x && v1.x <= v1.w) ? 1 : 0;
    int by1 = (-v1.w <= v1.y && v1.y <= v1.w) ? 2 : 0;
    int bz1 = (-v1.w <= v1.z && v1.z <= v1.w) ? 4 : 0;
    int bx2 = (-v2.w <= v2.x && v2.x <= v2.w) ? 8 : 0;
    int by2 = (-v2.w <= v2.y && v2.y <= v2.w) ? 16 : 0;
    int bz2 = (-v2.w <= v2.z && v2.z <= v2.w) ? 32 : 0;
    int bx3 = (-v3.w <= v3.x && v3.x <= v3.w) ? 64 : 0;
    int by3 = (-v3.w <= v3.y && v3.y <= v3.w) ? 128 : 0;
    int bz3 = (-v3.w <= v3.z && v3.z <= v3.w) ? 256 : 0;

    return bx1|bx2|bx3 | by1|by2|by3 | bz1|bz2|bz3;
}

void clip_triangle(std::vector<Vector4f>& vertexList, Vector4f plane)
{
    unsigned int edge0, edge1;
    unsigned int tri;
    size_t vertexListSize = vertexList.size();
    for(int tri=0; tri<vertexListSize; tri+=3){
	std::vector<Vector4f> polygon;
	for(edge0=tri+2, edge1=tri; edge1 < tri+3; edge0 = edge1++){
	    Vector4f point_current = vertexList[edge0];
	    Vector4f point_next = vertexList[edge1];

	    float dot0 = dot(point_current, plane) + point_current.w * plane.w;
	    float dot1 = dot(point_next,    plane) + point_next.w    * plane.w;

	    bool inside0 = dot0 >= 0.0f;
	    bool inside1 = dot1 >= 0.0f;

	    /* If start is inside, output it */
	    if(inside0)
		polygon.push_back(point_current);
	    if(inside0 != inside1){
		/* We're clipping an edge */
		float t=0.0f;
		float diff = 0.0f;
		/* swap vertices */
		if(!inside0){
		    std::swap(point_current, point_next);
		    std::swap(dot0, dot1);
		}
		diff = dot1 - dot0;
		if (std::abs(diff) > 1e-10)
		    t = dot0 / diff;
		Vector4f clip = point_current + (point_next - point_current) * t;
		//clip.w = point_current.w + 1.0f / (((1.0f / point_next.w) - (1.0f / point_current.w)) * t);
		clip.w = point_current.w +  (point_next.w - point_current.w) * t;
		polygon.push_back(clip);
	    }
	}
	/* Split the resulting polygon into triangles */
	size_t vcount = polygon.size();
	if(vcount < 3)
	    continue;
	for(int p=1; p<vcount-1; ++p){
	    polygon.push_back(polygon[0]);
	    polygon.push_back(polygon[p]);
	    polygon.push_back(polygon[p+1]);
	}
	std::copy(polygon.begin() + vcount, polygon.end(), std::back_inserter(vertexList));
    }
    vertexList.erase(vertexList.begin(), vertexList.begin() + vertexListSize);
}
