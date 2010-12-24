#include <vector>
#include <algorithm>
#include <linealg.h>
#include "clipplane.h"

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
            /* point.w is positive*/
	    /* v.n - d = 0 */
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
		diff = dot0 - dot1;
		if (std::abs(diff) > 1e-10)
		    t = dot0 / diff;
		Vector4f clip = point_current + (point_next - point_current) * t;
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
