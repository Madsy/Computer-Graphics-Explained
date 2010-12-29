#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <linealg.h>
#include <fixedpoint.h>
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

    y0 >>= 16;
    y1 >>= 16;

    while(y0 <= y1)
    {
	int PosX = ceilfp(x0);
        int EndX = ceilfp(x1) - (1<<16);
	
	Vector4i PosTex = tInterp.start;
	Vector4i SlopeTex;
	int xError = PosX - ceilfp(x0);
	int xDelta = x1 - x0;
	int column = y0*width;
                
        unsigned int texWidth = currentTexture->width;
        unsigned int texHeight = currentTexture->height;

	if(!xDelta){
	    SlopeTex = Vector4i(0,0,0);
	}
	else {
	    SlopeTex.x = ((long long)(tInterp.end.x - tInterp.start.x) << 16) / xDelta;
	    SlopeTex.y = ((long long)(tInterp.end.y - tInterp.start.y) << 16) / xDelta;
	}
                
	//Correct for offseted x coord
	//checked for positive xDelta, or else the slopes are 0 and can be ignored
	if(xError && xDelta){
	    PosTex.x   += ((long long)SlopeTex.x * xError) >> 16;
	    PosTex.y   += ((long long)SlopeTex.y * xError) >> 16;
	}
                
	PosX >>= 16;
	EndX >>= 16;

	for(;PosX <= EndX; ++PosX){
	    unsigned int s = ((long long)PosTex.x * (texWidth-1))>>16ULL;
	    unsigned int t = ((long long)PosTex.y * (texHeight-1))>>16ULL;

            const unsigned int* texture = &currentTexture->color[0];
	    unsigned int texel = texture[s + t*texWidth];
	    /* This if statement is needed for now, because the clipping is inaccurate */
	    if((PosX >= 0 && PosX < width) && (y0 >= 0 && y0 < height))
		buffer[PosX + column] = texel;
                    
	    PosTex.x   += SlopeTex.x;
	    PosTex.y   += SlopeTex.y;
	}
            
	x0 += vInterp.slope0.x;
	x1 += vInterp.slope1.x;
            
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
        
        Vector4i& v0 = vertexData[i+0];
        Vector4i& v1 = vertexData[i+1];
        Vector4i& v2 = vertexData[i+2];
    
	Vector4i& t0 = textureData[i+0];
        Vector4i& t1 = textureData[i+1];
        Vector4i& t2 = textureData[i+2];

        /* v0 and v1 is on top */        
        if(v0.y == v1.y){
            if(v1.x < v0.x){
                std::swap(v0, v1);
		std::swap(t0, t1);
	    }
            
            vertexStart = Vector4i(v0.x, ceilfp(v0.y),           0, 0);
            vertexEnd   = Vector4i(v1.x, ceilfp(v2.y) - (1<<16), 0, 0);
            vertexEdge0 = v2 - v0;
            vertexEdge1 = v2 - v1;

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
            vertexStart = Vector4i(v0.x, ceilfp(v0.y),           0, 0);
	    vertexEnd   = Vector4i(v0.x, ceilfp(v1.y) - (1<<16), 0, 0);
	    vertexEdge0 = v1 - v0;
            vertexEdge1 = v2 - v0;

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
            tcoordSlope1.x = ((long long)tcoordEdge1.x<<16) / vertexEdge1.y;
            tcoordSlope1.y = ((long long)tcoordEdge1.y<<16) / vertexEdge1.y;
        }
        
        
        int yError = vertexStart.y - v0.y;
        
        /* correct for offseted start y-coord */
        if(yError)
        {
            vertexStart.x += ((long long)vertexSlope0.x*yError) >> 16;
            vertexEnd.x += ((long long)vertexSlope1.x*yError) >> 16;
        
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
