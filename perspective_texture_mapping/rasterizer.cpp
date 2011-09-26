#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <linealg.h>
#include <fixedpoint.h>
#include "framebuffer.h"
#include "texture.h"
#include "myassert.h"


void TriangleSplit(
		   std::vector<Vector4i>& vertexData,
		   std::vector<Vector4i>& textureData
		   )
{
    unsigned int len = vertexData.size();
    for(unsigned int i = 0; i<len; i+=3)
    {
        Vector4i v0 = vertexData[i+0];
        Vector4i v1 = vertexData[i+1];
        Vector4i v2 = vertexData[i+2];
        
	Vector4i t0 = textureData[i+0];
        Vector4i t1 = textureData[i+1];
        Vector4i t2 = textureData[i+2];

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

	ASSERT(v0.z >= 0 && v0.z <= 65535);
	ASSERT(v1.z >= 0 && v1.z <= 65535);
	ASSERT(v2.z >= 0 && v2.z <= 65535);

	if( (v0.y == v1.y) && (v1.y == v2.y) ){
	    /* Ignore degenerate triangles */
            continue;
        } else if( (v0.y == v1.y) || (v1.y == v2.y)){
	    /* don't split if we already have a horisontal edge */
	    vertexData.push_back(v0);
	    vertexData.push_back(v1);
	    vertexData.push_back(v2);
	    textureData.push_back(t0);
	    textureData.push_back(t1);
	    textureData.push_back(t2);
            continue;
	}
        
        /* Split in two triangles */

        Vector4i vertexEdge0 = v1 - v0;
        Vector4i vertexEdge1 = v2 - v0;
	/* edge v2 - v0 for texture coordinates */
	Vector4i textureEdge = t2 - t0;

	int vertexSlopeX, vertexSlopeZ, vertexSlopeW;
	int textureSlopeS, textureSlopeT;

	if(!vertexEdge1.y){
	    vertexSlopeX = vertexSlopeZ = vertexSlopeW = 0;
	} else {
	    vertexSlopeX =  ((long long)vertexEdge1.x << 16) / vertexEdge1.y;
	    vertexSlopeZ =  ((long long)vertexEdge1.z << 16) / vertexEdge1.y;
	    vertexSlopeW =  ((long long)vertexEdge1.w << 16) / vertexEdge1.y;
	    textureSlopeS = ((long long)textureEdge.x << 16) / vertexEdge1.y;
	    textureSlopeT = ((long long)textureEdge.y << 16) / vertexEdge1.y;
	}
	
        //new point
	int x_new = v0.x + (((long long)vertexEdge0.y * vertexSlopeX) >> 16);
	int y_new = v1.y;

	int z_new = v0.z + (((long long)vertexEdge0.y * vertexSlopeZ) >> 16);
	int w_new = v0.w + (((long long)vertexEdge0.y * vertexSlopeW) >> 16);
	
	int s_new = t0.x + (((long long)vertexEdge0.y * textureSlopeS) >> 16);
	int t_new = t0.y + (((long long)vertexEdge0.y * textureSlopeT) >> 16);

        Vector4i v3(x_new, y_new, z_new, w_new);
	Vector4i t3(s_new, t_new, 0, 0);

	ASSERT(v3.z >= 0 && v3.z <= 65536);

	/* Triangulate the two new triangles */
	vertexData.push_back(v1);
	vertexData.push_back(v3);
	vertexData.push_back(v2);

	vertexData.push_back(v0);
	vertexData.push_back(v1);
	vertexData.push_back(v3);

	/* Same, but for the texture data */
	textureData.push_back(t1);
	textureData.push_back(t3);
        textureData.push_back(t2);

	textureData.push_back(t0);
	textureData.push_back(t1);
	textureData.push_back(t3);
    }
    vertexData.erase(vertexData.begin(), vertexData.begin() + len);
    textureData.erase(textureData.begin(), textureData.begin() + len);
}


/* Temporary structure for interpolated data.
 Used by TriangleScan() and DrawTriangle()*/

struct InterpData
{
    InterpData(const Vector4i& pStart,  const Vector4i& pEnd,
	       const Vector4i& pSlope0, const Vector4i& pSlope1,
	       const Vector4i& pEdge0,  const Vector4i& pEdge1) :
	start(pStart), end(pEnd),
	slope0(pSlope0), slope1(pSlope1),
	edge0(pEdge0), edge1(pEdge1)
    {}
    Vector4i start, end;
    Vector4i slope0, slope1;
    Vector4i edge0, edge1;
};


static void TriangleScan(
		  InterpData vInterp,
		  InterpData tInterp,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  )
{
    int x0 = vInterp.start.x;
    int x1 = vInterp.end.x;
    int y0 = vInterp.start.y;
    int y1 = vInterp.end.y;
    int z0 = vInterp.start.z;
    int z1 = vInterp.end.z;
    int w0 = vInterp.start.w;
    int w1 = vInterp.end.w;

    //ASSERT(z0 >= 0 && z0 <= 65536);
    //ASSERT(z1 >= 0 && z1 <= 65536);
    ASSERT(w0 != 0);
    ASSERT(w1 != 0);

    y0 >>= 16;
    y1 >>= 16;
    
    /* This correction is needed for now, because the clipping is inaccurate.
	 NOTE: This does not make clipping redundant. without clipping our values would overflow, or
	not fit into fixedpoint integers. */
    if(y0 < 0){
	int frYError = -y0;
	x0 += vInterp.slope0.x * frYError;
	x1 += vInterp.slope1.x * frYError;
	z0 += vInterp.slope0.z * frYError;
	z1 += vInterp.slope1.z * frYError;
	w0 += vInterp.slope0.w * frYError;
	w1 += vInterp.slope1.w * frYError;
	tInterp.start.x += tInterp.slope0.x * frYError;
	tInterp.start.y += tInterp.slope0.y * frYError;   
	tInterp.end.x   += tInterp.slope1.x * frYError;
	tInterp.end.y   += tInterp.slope1.y * frYError;
	y0 = 0;
    }
    if(y1 >= height)
	y1 = height-1;

    while(y0 <= y1)
    {
	int PosX = ceilfp(x0);
        int EndX = ceilfp(x1) - (1<<16);
	/* PosZ and PosW don't need end values, because they are
	   implicitly bound by the x and y values. In this regard, they are
	   just like any other interpolated scalars */
	int PosZ = z0;
	int PosW = w0;
	Vector4i PosTex = tInterp.start;
	Vector4i interpTex;
	Vector4i interpDepth;
	/* Ceil here to make sure we only render pixels *inside* the triangle */
	int xError = PosX - ceilfp(x0);
	int xDelta = x1 - x0;
	int column = y0*width;
                
        unsigned int texWidth = currentTexture->width;
        unsigned int texHeight = currentTexture->height;

	/* These are our interpolation values for the scanline */
	if(!xDelta){
	    interpTex = Vector4i(0,0,0,0);
	    interpDepth = Vector4i(0,0,0,0);
	}
	else {
	    interpDepth.z = ((long long)(z1 - z0) << 16) / xDelta;
	    interpDepth.w = ((long long)(w1 - w0) << 16) / xDelta;
	    interpTex.x = ((long long)(tInterp.end.x - tInterp.start.x) << 16) / xDelta;
	    interpTex.y = ((long long)(tInterp.end.y - tInterp.start.y) << 16) / xDelta;
	}
                
	//Correct for offseted x coord
	//checked for positive xDelta, or else the slopes are 0 and can be ignored
	if(xError && xDelta){
	    PosZ       += ((long long)interpDepth.z * xError) >> 16;
	    PosW       += ((long long)interpDepth.w * xError) >> 16;
	    PosTex.x   += ((long long)interpTex.x * xError) >> 16;
	    PosTex.y   += ((long long)interpTex.y * xError) >> 16;
	}
        
	/* PosX and EndX are from now on proper integers (not fixedpoint) */
	PosX >>= 16;
	EndX >>= 16;
	
	/* This correction is needed for now, because the clipping is inaccurate.
	 NOTE: This does not make clipping redundant. without clipping our values would overflow, or
	not fit into fixedpoint integers. */
	if(PosX < 0){
	    /* No shifts here. PosX and frXError are not fixedpoint anymore */
	    int frXError = -PosX;
	    PosTex.x   += interpTex.x * frXError;
	    PosTex.y   += interpTex.y * frXError;
	    PosZ       += interpDepth.z * frXError;
	    PosW       += interpDepth.w * frXError;
            PosX  = 0;
	}
	if(EndX >= width)
	    EndX = width - 1;

	for(;PosX <= EndX; ++PosX){
	    int negW, Zpc;
	    
	    /* perspective correct z*/
	    Zpc = ((long long)PosW * PosZ)>>16;
	    
	    if(Zpc <= depthbuffer.data[PosX + column]){
		unsigned int s,t;
		s = ((long long)PosTex.x * PosW)>>16;
		t = ((long long)PosTex.y * PosW)>>16;
		s = ((long long)s * (texWidth-1))>>16LL;
		t = ((long long)t * (texHeight-1))>>16LL;
		unsigned int texel = currentTexture->color[s + t*texWidth];
		depthbuffer.data[PosX + column] = Zpc;
		buffer[PosX + column] = texel;
	    }
            
	    PosZ       += interpDepth.z;
	    PosW       += interpDepth.w;
	    PosTex.x   += interpTex.x;
	    PosTex.y   += interpTex.y;
	}
            
	x0 += vInterp.slope0.x;
	x1 += vInterp.slope1.x;
	z0 += vInterp.slope0.z;
	z1 += vInterp.slope1.z;
	w0 += vInterp.slope0.w;
	w1 += vInterp.slope1.w;

	tInterp.start.x += tInterp.slope0.x;
	tInterp.start.y += tInterp.slope0.y;   
	tInterp.end.x   += tInterp.slope1.x;
	tInterp.end.y   += tInterp.slope1.y;
        
	++y0;
    }
}


void DrawTriangle(
		  std::vector<Vector4i>& vertexData,
		  std::vector<Vector4i>& textureData,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  )
{
    Vector4i vertexStart, vertexEnd;
    Vector4i vertexEdge0, vertexEdge1;
    Vector4i vertexSlope0, vertexSlope1;

    Vector4i tcoordStart, tcoordEnd;
    Vector4i tcoordEdge0, tcoordEdge1;
    Vector4i tcoordSlope0, tcoordSlope1;
    
    for(unsigned int i=0; i<vertexData.size(); i+=3){
        
        Vector4i v0 = vertexData[i+0];
        Vector4i v1 = vertexData[i+1];
        Vector4i v2 = vertexData[i+2];
    
	Vector4i t0 = textureData[i+0];
        Vector4i t1 = textureData[i+1];
        Vector4i t2 = textureData[i+2];

	/* vN.w is really 1/w. It's inverted in project() in linealg.h */
	
	v0.z = ((long long)v0.z*v0.w)>>16;
	v1.z = ((long long)v1.z*v1.w)>>16;
	v2.z = ((long long)v2.z*v2.w)>>16;
	
        /* v0 and v1 is on top */        
        if(v0.y == v1.y){
            if(v1.x < v0.x){
                std::swap(v0, v1);
		std::swap(t0, t1);
	    }
            
            vertexStart = Vector4i(v0.x, ceilfp(v0.y),           v0.z, v0.w);
            vertexEnd   = Vector4i(v1.x, ceilfp(v2.y) - (1<<16), v1.z, v1.w);
            vertexEdge0 = v2 - v0;
            vertexEdge1 = v2 - v1;
	    vertexEdge0.w = v2.w - v0.w;
	    vertexEdge1.w = v2.w - v1.w;
            tcoordStart = t0;
            tcoordEnd   = t1;
            tcoordEdge0 = t2 - t0;
            tcoordEdge1 = t2 - t1;
        }
        else { /* p0 is on top, p1 and p2 is below */
            if(v2.x < v1.x){
                std::swap(v1, v2);
		std::swap(t1, t2);
	    }
            vertexStart = Vector4i(v0.x, ceilfp(v0.y),           v0.z, v0.w);
	    vertexEnd   = Vector4i(v0.x, ceilfp(v1.y) - (1<<16), v0.z, v0.w);
	    vertexEdge0 = v1 - v0;
            vertexEdge1 = v2 - v0;
	    vertexEdge0.w = v1.w - v0.w;
            vertexEdge1.w = v2.w - v0.w;
            tcoordStart = t0;
            tcoordEnd   = t0;
            tcoordEdge0 = t1 - t0;
            tcoordEdge1 = t2 - t0;
        }
        
        /* slope for leftmost edge */
        if(!vertexEdge0.y){
            vertexSlope0 = Vector4i(0,0,0,0);
            tcoordSlope0 = Vector4i(0,0,0,0);
        }
        else{
            vertexSlope0.x = ((long long)vertexEdge0.x<<16) / vertexEdge0.y;
	    vertexSlope0.z = ((long long)vertexEdge0.z<<16) / vertexEdge0.y;
	    vertexSlope0.w = ((long long)vertexEdge0.w<<16) / vertexEdge0.y;
            tcoordSlope0.x = ((long long)tcoordEdge0.x<<16) / vertexEdge0.y;
            tcoordSlope0.y = ((long long)tcoordEdge0.y<<16) / vertexEdge0.y;
        }
        
        /* slope for rightmost edge */
        if(!vertexEdge1.y){
            vertexSlope1 = Vector4i(0,0,0,0);
            tcoordSlope1 = Vector4i(0,0,0,0);
        }
        else{
            vertexSlope1.x = ((long long)vertexEdge1.x<<16) / vertexEdge1.y;
	    vertexSlope1.z = ((long long)vertexEdge1.z<<16) / vertexEdge1.y;
	    vertexSlope1.w = ((long long)vertexEdge1.w<<16) / vertexEdge1.y;
            tcoordSlope1.x = ((long long)tcoordEdge1.x<<16) / vertexEdge1.y;
            tcoordSlope1.y = ((long long)tcoordEdge1.y<<16) / vertexEdge1.y;
        }
        
        
        int yError = vertexStart.y - v0.y;
        
        /* correct for offseted start y-coord */
        if(yError)
        {
            vertexStart.x += ((long long)vertexSlope0.x*yError) >> 16;
	    vertexStart.z += ((long long)vertexSlope0.z*yError) >> 16;
	    vertexStart.w += ((long long)vertexSlope0.w*yError) >> 16;
            vertexEnd.x +=   ((long long)vertexSlope1.x*yError) >> 16;
	    vertexEnd.z +=   ((long long)vertexSlope1.z*yError) >> 16;
	    vertexEnd.w +=   ((long long)vertexSlope1.w*yError) >> 16;

            tcoordStart.x += ((long long)tcoordSlope0.x*yError) >> 16;
            tcoordStart.y += ((long long)tcoordSlope0.y*yError) >> 16;
            tcoordEnd.x   += ((long long)tcoordSlope1.x*yError) >> 16;
            tcoordEnd.y   += ((long long)tcoordSlope1.y*yError) >> 16;
        }

	InterpData vertexInterp(vertexStart, vertexEnd,
				vertexSlope0, vertexSlope1,
				vertexEdge0, vertexEdge1);
	InterpData textureInterp(tcoordStart, tcoordEnd,
				 tcoordSlope0, tcoordSlope1,
				 tcoordEdge0, tcoordEdge1);

	TriangleScan(vertexInterp, textureInterp, buffer, width, height);
    }
}
