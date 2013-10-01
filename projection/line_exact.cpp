#include <linealg.h>
#include "line.h"
#include <assert.h>

/* draw an y-major line, that is -1 < x/y < 1
   Called internally by drawLine() */
static void drawLineYMajor(const Vector2i& p1, const Vector2i& p2,
						   unsigned int* buffer, int width, int height)
{	
	assert(p2.y >= p1.y);
	Vector2i edge = p2-p1;
	int xAccum = 0;
	int x = p1.x;
	int sign = edge.x > 0 ? 1 : -1;
	edge.x = std::abs(edge.x);	
	for(int y = p1.y; y < p2.y; ++y){
		/* accumulate edge.x / edge.y */
		xAccum += edge.x;
		while(xAccum >= edge.y){
			x += sign;
			xAccum -= edge.y;			
		}		
		buffer[x + y * width] = 0xFFFFFFFF;		
	}
}

/* draw an x-major line, that is -1 < y/x < 1
   Called internally by drawLine() */
static void drawLineXMajor(const Vector2i& p1, const Vector2i& p2,
						   unsigned int* buffer, int width, int height)
{
	assert(p2.x >= p1.x);
	Vector2i edge = p2 - p1;
	int yAccum = 0;
	int y = p1.y;
	int sign = edge.y > 0 ? 1 : -1;
	edge.y = std::abs(edge.y);	
	for(int x = p1.x; x < p2.x; ++x){
		/* accumulate edge.y / edge.x */
		yAccum += edge.y;
		while(yAccum >= edge.x){
			y += sign;
			yAccum -= edge.x;			
		}		
		buffer[x + y * width] = 0xFFFFFFFF;		
	}
}

/* A straight vertical line, which is a special case. The y direction is 0, so x/y = Inf
   Called internally by drawLine() */
static void drawLineVertical(Vector4f v1, Vector4f v2,
							 unsigned int* buffer, int width, int height)
{
    if(v2.y < v1.y)
		std::swap(v1, v2);
    int x = v1.x;
    int y1 = v1.y;
    int y2 = v2.y;
	
    for(int y=v1.y; y<v2.y; ++y)
		buffer[x + y*width] = 0xFFFFFFFF;
}

/* A straight horisontal line, which is a special case. The x direction is 0, so y/x = Inf
   Called internally by drawLine() */
static void drawLineHorisontal(Vector4f v1, Vector4f v2,
							   unsigned int* buffer, int width, int height)
{
    if(v2.x < v1.x)
		std::swap(v1, v2);
    int y = v1.y;
    int x1 = v1.x;
    int x2 = v2.x;
	
    for(int x=v1.x; x<v2.x; ++x)
		buffer[x + y*width] = 0xFFFFFFFF;
}

/* Our externally visible function, which draws lines with the Bresenham algorithm */
void drawLine(const Vector4f& v1, const Vector4f& v2,
			  unsigned int* buffer, int width, int height)
{
	Vector2i p1(v1.x, v1.y);
	Vector2i p2(v2.x, v2.y);
	
	/* Points downwards, sign dependent on x */	
	Vector2i edge = p2 - p1;
	
	if(!edge.x && !edge.y)
		return;
		
	if(!edge.y){
	    /* When drawing horisontal lines, v2 must have the largest x. We iterate from the
		   smallest x to the largest x. Remember that our framebuffer holds pixels in increasing x */
		if(p2.x < p1.x)
			std::swap(p1, p2);
		drawLineHorisontal(v1, v2, buffer, width, height);
	} else if(!edge.x){
		if(p2.y < p1.y)
			std::swap(p1, p2);
		drawLineVertical(v1, v2, buffer, width, height);
	} else {
		/* sloped line */
		
		/* y always positive, sign depends on x */
		if(std::abs(edge.x) < std::abs(edge.y)){
			if(p2.y < p1.y)
				std::swap(p1, p2);
			drawLineYMajor(p1, p2, buffer, width, height);
		} else {
			/* x major */
			if(p2.x < p1.x)
				std::swap(p1, p2);			
			drawLineXMajor(p1, p2, buffer, width, height);
		}
	}
	return;
}
