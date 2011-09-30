#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <linealg.h>
#include <fixedpoint.h>
#include "framebuffer.h"
#include "texture.h"
#include "myassert.h"



/*
Perspective correct interpolation:
u0 = u0/w
v0 = v0/w
u1 = u1/w
v1 = v1/w
w0 = 1/w0
w1 = 1/w1

u' = (u0 + (u1-u0)*t) / (w0 + (w1-w0)*t);
v' = (v0 + (v1-v0)*t) / (w0 + (w1-w0)*t);

*/


/*
        unsigned int texWidth = currentTexture->width;
        unsigned int texHeight = currentTexture->height;
		unsigned int texel = currentTexture->color[s + t*texWidth];
		depthbuffer.data[PosX + column] = Zpc;


 */

struct scanline_t
{
    float f_xstart, f_xend;
    float f_zstart, f_zend;
    float f_wstart, f_wend;
    float f_ustart, f_uend;
    float f_vstart, f_vend;
};

static void drawScanLine(unsigned int* cbuffer,
		  int width, int height,
		  int y,
		  scanline_t* scan)
{
    int xstart, xend, col;
    unsigned int x, texWidth, texHeight;
    float edge_x, xDelta, xError;
    float edge_u, edge_v, slope_u, slope_v;
    float edge_z, edge_w, slope_z, slope_w;
    float w;
    unsigned int u,v, texel, texcol;
    unsigned short z, *zbuffer;

    xstart = std::ceil(scan->f_xstart);
    xend   = std::ceil(scan->f_xend) - 1;
    col = y*width;

    /*
    if(xstart > xend){
	printf("xstart: %d, xend: %d\n", xstart, xend);
	ASSERT(!"xstart > xend");
    }
    */

    ASSERT(scan->f_xend > 0);
    edge_x = scan->f_xend - scan->f_xstart;
    edge_z = scan->f_zend - scan->f_zstart;
    edge_w = scan->f_wend - scan->f_wstart;
    edge_u = scan->f_uend - scan->f_ustart;
    edge_v = scan->f_vend - scan->f_vstart;

    if(edge_x > 1.0f)
	xDelta = 1.0f / edge_x; // x slope; scalar t used for interpolation
    else 
	xDelta = 0.0f;

    slope_z = edge_z * xDelta;
    slope_w = edge_w * xDelta;
    slope_u = edge_u * xDelta;
    slope_v = edge_v * xDelta;

    /* correction for fill convention */
    xError = (float)xstart - scan->f_xstart;
    scan->f_zstart += slope_z * xError;
    scan->f_wstart += slope_w * xError;
    scan->f_ustart += slope_u * xError;
    scan->f_vstart += slope_v * xError;

    /* offset z-buffer and color buffer */
    cbuffer += col;
    zbuffer = &depthbuffer.data[col];
    /* texture map */
    const unsigned int* texture = &currentTexture->color[0];
    texWidth = currentTexture->width;
    texHeight = currentTexture->height;

    float zinterp = scan->f_zstart;
    float winterp = scan->f_wstart;    
    float uinterp = scan->f_ustart;
    float vinterp = scan->f_vstart;

    for(x=xstart; x<=xend; ++x){
	ASSERT(xend > 0);
	z = zinterp * 65535.0f;
	if(z < zbuffer[x]){
	    zbuffer[x] = z;
	    ASSERT(std::abs(winterp) > 0.0f);
	    w = 1.0f / winterp;
	    u = uinterp * w * (float)texWidth;
	    v = vinterp * w * (float)texHeight;
	    texcol = v*texWidth;

	    cbuffer[x] = texture[u + texcol];
	}
	/* final interpolation of data */
	zinterp += slope_z;
	winterp += slope_w;
	uinterp += slope_u;
	vinterp += slope_v;
    }
}

/************************************************ OBS OBS OBS w = 1/w!! ******************************* */

void DrawTriangle(
		  std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& textureData,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  )
{
    const float eps = 0.00001f;
    float yDelta[3];
    int isMiddlePointRight;

    Vector4f vertexEdge0, vertexEdge1, vertexEdge2;
    Vector4f vertexSlope0, vertexSlope1, vertexSlope2;

    Vector4f tcoordEdge0, tcoordEdge1, tcoordEdge2;
    Vector4f tcoordSlope0, tcoordSlope1, tcoordSlope2;
    
    for(unsigned int i=0; i<vertexData.size(); i+=3){
        
        Vector4f& v0 = vertexData[i+0];
        Vector4f& v1 = vertexData[i+1];
        Vector4f& v2 = vertexData[i+2];
    
	Vector4f& t0 = textureData[i+0];
        Vector4f& t1 = textureData[i+1];
        Vector4f& t2 = textureData[i+2];

	int yStart, yEnd;
	scanline_t scan, scantmp;
	float yError;

	/* sort from top to bottom */
	if(v0.y > v1.y){
	    std::swap(v0, v1);
	    std::swap(t0, t1);
	}
	if(v1.y > v2.y){
	    std::swap(v1, v2);
	    std::swap(t1, t2);
	}
	if(v0.y > v1.y){
	    std::swap(v0, v1);
	    std::swap(t0, t1);
	}
	
	vertexEdge0 = v2 - v0;
	vertexEdge1 = v1 - v0;
	vertexEdge2 = v2 - v1;
	/* operator- does not assign w properly */
	vertexEdge0.w = v2.w - v0.w;
	vertexEdge1.w = v1.w - v0.w;
	vertexEdge2.w = v2.w - v1.w;

        tcoordEdge0 = t2 - t0;
        tcoordEdge1 = t1 - t0;
        tcoordEdge2 = t2 - t1;

	/* huge if the triangle is narrow (not very tall)
	   very small if the triangle is tall */
	if(std::abs(vertexEdge0.y) > eps)
	    yDelta[0] = 1.0f / vertexEdge0.y;
	else
	    yDelta[0] = 0.0f;
	if(std::abs(vertexEdge1.y) > eps)
	    yDelta[1] = 1.0f / vertexEdge1.y;
	else
	    yDelta[1] = 0.0f;
	if(std::abs(vertexEdge2.y) > eps)
	    yDelta[2] = 1.0f / vertexEdge2.y;
	else
	    yDelta[2] = 0.0f;

	/* slopes for interpolated scalars */
	vertexSlope0.x = vertexEdge0.x * yDelta[0];
	vertexSlope0.z = vertexEdge0.z * yDelta[0];
	vertexSlope0.w = vertexEdge0.w * yDelta[0];
	vertexSlope1.x = vertexEdge1.x * yDelta[1];
	vertexSlope1.z = vertexEdge1.z * yDelta[1];
	vertexSlope1.w = vertexEdge1.w * yDelta[1];
	vertexSlope2.x = vertexEdge2.x * yDelta[2];
	vertexSlope2.z = vertexEdge2.z * yDelta[2];
	vertexSlope2.w = vertexEdge2.w * yDelta[2];

	tcoordSlope0.x = tcoordEdge0.x * yDelta[0];
	tcoordSlope0.y = tcoordEdge0.y * yDelta[0];
	tcoordSlope1.x = tcoordEdge1.x * yDelta[1];
	tcoordSlope1.y = tcoordEdge1.y * yDelta[1];
	tcoordSlope2.x = tcoordEdge2.x * yDelta[2];
	tcoordSlope2.y = tcoordEdge2.y * yDelta[2];

	/* Is the middle point on the left- or right side ? */
	if(((-vertexEdge0.x*vertexEdge1.y) - (-vertexEdge0.y*vertexEdge1.x)) > 0) isMiddlePointRight = 0;
	else isMiddlePointRight = 1;

	/**************************************************************************************************************
                                                           TOP-PART TRIANGLE
	***************************************************************************************************************/
	/* start values */
	yStart = std::ceil(v0.y);
	yEnd = std::ceil(v1.y) - 1;
	scan.f_xstart = v0.x;
	scan.f_xend = v0.x;
	/* z should be linearly interpolated (no perspective correction) */
        scan.f_zstart = v0.z;
	scan.f_zend = v0.z;
	/* This is really 1/w, which is linear in screen space */
	scan.f_wstart = v0.w;
	scan.f_wend = v0.w;
	/* perspective correct texture mapping. u and v are already divided by w in main.cpp!*/
	scan.f_ustart = t0.x; //* v0.w;
	scan.f_vstart = t0.y; //* v0.w;
	scan.f_uend   = t0.x; //* v0.w;
	scan.f_vend   = t0.y; //* v0.w;

	/* compensate for the fill convention */
	yError = (float)yStart - v0.y;
	
	scan.f_xstart += vertexSlope0.x*yError;
	scan.f_xend   += vertexSlope1.x*yError;
	scan.f_zstart += vertexSlope0.z*yError;
	scan.f_zend   += vertexSlope1.z*yError;
	scan.f_wstart += vertexSlope0.w*yError;
	scan.f_wend   += vertexSlope1.w*yError;
	scan.f_ustart += tcoordSlope0.x*yError;
	scan.f_uend   += tcoordSlope1.x*yError;
	scan.f_vstart += tcoordSlope0.y*yError;
	scan.f_vend   += tcoordSlope1.y*yError;

	/* render */
	if(scan.f_xstart < scan.f_xend){ //isMiddlePointRight
	    for(int y=yStart; y<=yEnd; ++y){
		scantmp = scan;
		ASSERT(scantmp.f_xstart <= scantmp.f_xend);
		//printf("(1) xstart: %f, xend: %f\n", scantmp.f_xstart, scantmp.f_xend);
		drawScanLine(buffer, width, height, y, &scantmp);
		scan.f_xstart   += vertexSlope0.x;
		scan.f_xend     += vertexSlope1.x;
		scan.f_zstart   += vertexSlope0.z;
		scan.f_zend     += vertexSlope1.z;
		scan.f_wstart   += vertexSlope0.w;
		scan.f_wend     += vertexSlope1.w;
		scan.f_ustart += tcoordSlope0.x;
		scan.f_uend   += tcoordSlope1.x;
		scan.f_vstart += tcoordSlope0.y;
		scan.f_vend   += tcoordSlope1.y;
	    }
	} else {
	    for(int y=yStart; y<=yEnd; ++y){
		scantmp = scan;
		std::swap(scantmp.f_xstart, scantmp.f_xend);
		std::swap(scantmp.f_zstart, scantmp.f_zend);
		std::swap(scantmp.f_wstart, scantmp.f_wend);
		std::swap(scantmp.f_ustart, scantmp.f_uend);
		std::swap(scantmp.f_vstart, scantmp.f_vend);
		//ASSERT(scantmp.f_xstart <= scantmp.f_xend);
		//printf("(1) xstart: %f, xend: %f\n", scantmp.f_xstart, scantmp.f_xend);
		drawScanLine(buffer, width, height, y, &scantmp);
		scan.f_xstart   += vertexSlope0.x;
		scan.f_xend     += vertexSlope1.x;
		scan.f_zstart   += vertexSlope0.z;
		scan.f_zend     += vertexSlope1.z;
		scan.f_wstart   += vertexSlope0.w;
		scan.f_wend     += vertexSlope1.w;
		scan.f_ustart += tcoordSlope0.x;
		scan.f_uend   += tcoordSlope1.x;
		scan.f_vstart += tcoordSlope0.y;
		scan.f_vend   += tcoordSlope1.y;		
	    }
	}
	/**************************************************************************************************************
                                                           BOTTOM-PART TRIANGLE
	***************************************************************************************************************/

	/* start values */
	/* We interpolate the values along the major edge (slope0) to find the new point at v1.y */
	yStart = std::ceil(v1.y);
	yEnd = std::ceil(v2.y) - 1;
	scan.f_xstart = v0.x + vertexSlope0.x * vertexEdge1.y;
	scan.f_xend = v1.x;

        scan.f_zstart = v0.z + vertexSlope0.z * vertexEdge1.y;
	scan.f_zend = v1.z;

	scan.f_wstart = v0.w + vertexSlope0.w * vertexEdge1.y;
	scan.f_wend = v1.w;

	//scan.f_ustart = (t0.x * v0.w) + tcoordSlope0.x * vertexEdge1.y;
	//scan.f_vstart = (t0.y * v0.w) + tcoordSlope0.y * vertexEdge1.y;
	scan.f_ustart = (t0.x + tcoordSlope0.x * vertexEdge1.y);
	scan.f_vstart = (t0.y + tcoordSlope0.y * vertexEdge1.y);
	scan.f_uend   = t1.x;
	scan.f_vend   = t1.y;

	/* compensate for the fill convention */
	yError = (float)yStart - v1.y;
	
	scan.f_xstart += vertexSlope0.x*yError;
	scan.f_xend   += vertexSlope2.x*yError;
	scan.f_zstart += vertexSlope0.z*yError;
	scan.f_zend   += vertexSlope2.z*yError;
	scan.f_wstart += vertexSlope0.w*yError;
	scan.f_wend   += vertexSlope2.w*yError;
	scan.f_ustart += tcoordSlope0.x*yError;
	scan.f_uend   += tcoordSlope2.x*yError;
	scan.f_vstart += tcoordSlope0.y*yError;
	scan.f_vend   += tcoordSlope2.y*yError;

	if(scan.f_xstart < scan.f_xend){ //!isMiddlePointRight
	    for(int y=yStart; y<=yEnd; ++y){
		scantmp = scan;
		drawScanLine(buffer, width, height, y, &scantmp);
		scan.f_xstart   += vertexSlope0.x;
		scan.f_xend     += vertexSlope2.x;
		scan.f_zstart   += vertexSlope0.z;
		scan.f_zend     += vertexSlope2.z;
		scan.f_wstart   += vertexSlope0.w;
		scan.f_wend     += vertexSlope2.w;
		scan.f_ustart += tcoordSlope0.x;
		scan.f_uend   += tcoordSlope2.x;
		scan.f_vstart += tcoordSlope0.y;
		scan.f_vend   += tcoordSlope2.y;
	    }
	} else {
	    for(int y=yStart; y<=yEnd; ++y){
		scantmp = scan;
		std::swap(scantmp.f_xstart, scantmp.f_xend);
		std::swap(scantmp.f_zstart, scantmp.f_zend);
		std::swap(scantmp.f_wstart, scantmp.f_wend);
		std::swap(scantmp.f_ustart, scantmp.f_uend);
		std::swap(scantmp.f_vstart, scantmp.f_vend);
		drawScanLine(buffer, width, height, y, &scantmp);
		scan.f_xstart   += vertexSlope0.x;
		scan.f_xend     += vertexSlope2.x;
		scan.f_zstart   += vertexSlope0.z;
		scan.f_zend     += vertexSlope2.z;
		scan.f_wstart   += vertexSlope0.w;
		scan.f_wend     += vertexSlope2.w;
		scan.f_ustart += tcoordSlope0.x;
		scan.f_uend   += tcoordSlope2.x;
		scan.f_vstart += tcoordSlope0.y;
		scan.f_vend   += tcoordSlope2.y;		
	    }
	}
	//SDL_Flip(screen);
    }
}
