#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <linealg.h>
#include "myassert.h"

void TriangleSplit(std::vector<Vector4f>& triangle)
{
    std::vector<Vector4f> buf;

    size_t len =  triangle.size();
    for(unsigned int i=0; i<len; i+=3){
	Vector4f* v0 = &triangle[i+0];
	Vector4f* v1 = &triangle[i+1];
	Vector4f* v2 = &triangle[i+2];
    
	if(v0->y > v1->y)
	    std::swap(v0, v1);
	if(v1->y > v2->y)
	    std::swap(v1, v2);
	if(v0->y > v1->y)
	    std::swap(v0, v1);

	int v0v1eq = !((int)v1->y - (int)v0->y);
	int v1v2eq = !((int)v2->y - (int)v1->y);
	int index = v0v1eq | (v1v2eq<<1);

	Vector4f edge, newpoint;
	float slope;
	int numlines;
	
	/* Based on the bits from the equivalence tests, we get four cases */
	switch(index)
	{
	    /* index 0 : We have a split triangle */
	case 0:
	    edge.x = (v2->x - v0->x);
	    edge.y = (v2->y - v0->y);
	    
	    if(std::abs(edge.y) > 0.00000001f)
		slope = edge.x / edge.y;
	    else
		slope = 0;
	    
	    numlines = v1->y - v0->y;
	    newpoint = Vector4f(v0->x + (float)numlines*slope, v1->y, 0.0f, 1.0f);

	    buf.push_back(*v0);
	    buf.push_back(newpoint);
	    buf.push_back(*v1);

	    buf.push_back(*v1);
	    buf.push_back(newpoint);
	    buf.push_back(*v2);

	break;
	    /* index 1 : v0 and v1 is equal. top flat edge */
	case 1:
	    /* index 2 : v1 and v2 is equal. bottom flat edge */
	case 2:
	    buf.push_back(*v0);
	    buf.push_back(*v1);
	    buf.push_back(*v2);
	break;
	    /* index 3 : All vertices are equal in y, which is a degenerate triangle. */
	case 3:
	    /* discard it */
	break;
	}
    }
    ASSERT(!buf.empty());
    triangle = buf;
}

static float minSlope = 1000.0f;
static float maxSlope = -1000.0f;

static void TriangleScan(
		  Vector2i start,
		  Vector2i end,
		  int slope0,
		  int slope1,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height,
		  unsigned int color
		  )
{
    int xstart, xend;
    
    ASSERT(start.y <= end.y);

    float s0 = (slope0 / 65536.0f);
    float s1 = (slope1 / 65536.0f);

    //printf("slope 0: %f\n", s0 );
    //printf("slope 1: %f\n", s1 );

    if(s0 < minSlope)
	minSlope = s0;
    if(s1 < minSlope)
	minSlope = s1;
    if(s0 > maxSlope)
	maxSlope = s0;
    if(s1 > maxSlope)
	maxSlope = s1;

    for(;start.y < end.y; ++start.y)
    {
	/* Make sure we scan in the right direction */
    
	xstart = (start.x>>16);
	xend = (end.x>>16);

	if(xstart > xend){
	    std::swap(xstart, xend);
	    std::swap(start.x, end.x);
	    std::swap(slope0, slope1);
	}

	for(;xstart<=xend; ++xstart){
	    /* Draw here */
	    buffer[start.y*width + xstart] = color;
	}
	//SDL_Surface* screen = SDL_GetVideoSurface();
	//SDL_Flip(screen);
	//buffer[start.y*width + xstart] = 0xFFFFFFFF;
	//buffer[start.y*width + xend] = 0xFFFFFFFF;
	/* increase interpolants and the two edges*/
	start.x += slope0;
	end.x += slope1;
    }
}


void DrawTriangle(
		  std::vector<Vector4f>& triangle,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  )
{
    
    Vector2i edge0, edge1;
    Vector2i start, end;
    int slope0, slope1;
    bool split = true;

    // unsigned int color[4] = {0xFFFF0000, 0xFF0000FF, 0xFF00FFFF, 0xFFFF00FF};
    // unsigned int cindex = 0;

    TriangleSplit(triangle);

    for(unsigned int i=0; i<triangle.size(); i+=3){
        
        const Vector4f* v0 = &triangle[i+0];
        const Vector4f* v1 = &triangle[i+1];
        const Vector4f* v2 = &triangle[i+2];
    
	if(v0->y > v1->y)
            std::swap(v0, v1);
        if(v1->y > v2->y)
            std::swap(v1, v2);
        if(v0->y > v1->y)
            std::swap(v0, v1);

	ASSERT(v0->y <= v1->y);
	ASSERT(v1->y <= v2->y);
	ASSERT(v0->y <  v2->y);

	int v0v1eq = v1->y - v0->y;
	int v1v2eq = v2->y - v1->y;

        /* p0 and p1 is on top. v0v1eq is simply the v0->y and v1-y> with truncation */
        if(!v0v1eq){
	    /* Check for degenerate triangle */
	    if(!v1v2eq)
		continue;
	    /* left edge Q15.16 fixedpoint */
	    edge0.x = (v2->x - v0->x) * 65536.0f;
	    edge0.y = (v2->y - v0->y);
	    /* right edge Q15.16 fixedpoint */
	    edge1.x = (v2->x - v1->x) * 65536.0f;
	    edge1.y = (v2->y - v1->y);
	    /* A straight edge on the top, which means no split triangle */
	    split = false;
	    
	    /* Set x and y start and end point */
	    start = Vector2i(v0->x * 65536.0f, v0->y);
	    end   = Vector2i(v1->x * 65536.0f, v2->y);
	    
        } else { /* p0 is on top, p1 and p2 is below */
	    /* If the bottom edge is a straight line, we have no split */
	    if(!v1v2eq){
		split = false;
		/* Set x and y start and end point */
		start = Vector2i(v0->x * 65536.0f, v0->y);
		end   = Vector2i(v0->x * 65536.0f, v2->y);
	    } else {
		/* Set x and y start and end point.
		 We have a split. The y1=v2 edge is drawn later*/
		start = Vector2i(v0->x * 65536.0f, v0->y);
		end   = Vector2i(v0->x * 65536.0f, v1->y);
	    }
            /* left edge Q15.16 fixedpoint */
	    edge0.x = (v1->x - v0->x) * 65536.0f;
	    edge0.y = (v1->y - v0->y);
	    /* right edge Q15.16 fixedpoint */
	    edge1.x = (v2->x - v0->x) * 65536.0f;
	    edge1.y = (v2->y - v0->y);
        }

	/* If split wasn't set to false, (the most common case) then we have
	   a split triangle. None of the three vertices have the same y, so we must split
	   the triangle in two and draw it twice. */
        
	/* ** Slopes for left and right edges. Each vertex attribute has its own slope too. **
	   ** slope_attribute.x = delta_attribute.x / edge.y                                **
	   ** slope_attribute.y = delta_attribute.y / edge.y                                **/

        /* slope for leftmost edge */
        if(!edge0.y){
	    /* Infinite slope. We can't divide by zero */
            slope0 = 0;
        }
        else{
	    /* slope = edge.x / edge.y, Q15.16 fixedpoint */
	    slope0 = edge0.x / edge0.y;
        }
        
        /* slope for rightmost edge */
        if(!edge1.y){
	    /* Infinite slope. We can't divide by zero */
            slope1 = 0;
        }
        else{
	    /* slope = edge.x / edge.y, Q15.16 fixedpoint */
	    slope1 = edge1.x / edge1.y;
        }
        
	TriangleScan(start, end, slope0, slope1, buffer, width, height, 0xFFFFFFFF);
	//cindex++;
    }
}


















































/*

	if(!split)
	    continue;

	int numlines = 1 + v1->y - v0->y;

	start.y += numlines;
	start.x += (numlines * slope1);


	edge0.x = (v2->x - v1->x) * 65536.0f;
	edge0.y = (v2->y - v1->y);
	if(!edge0.y)
            slope0 = 0;
        else
	    slope0 = edge0.x / edge0.y;

 
	end = Vector2i(v1->x * 65536.0f, v2->y);
	
	TriangleScan(start, end, slope1, slope0, buffer, width, height, color);
*/
