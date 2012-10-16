#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <linealg.h>
#include <fixedpoint.h>
#include "rasterizer.h"
#include "framebuffer.h"
#include "texture.h"
#include "myassert.h"

inline int fpceil(int fp)
{
  return (fp & 65535) ? ((fp & ~65535) + 65536) : fp;
}

static void drawScanLine(unsigned int* cbuffer,
		  int width,
		  int height,
		  int y,
		  int x1, int x2,
		  int z1, int z2,
		  int w1, int w2,
		  int s1, int s2,
		  int t1, int t2)
{
  unsigned short* zbuffer;
  unsigned short z;
  int deltaX, deltaZ, deltaW, deltaS, deltaT;
  int slopeZ, slopeW, slopeS, slopeT;
  int zStart, zEnd, wStart, wEnd, sStart, sEnd, tStart, tEnd;
  int xError;
  int w, s, t;
  int texWidth, texHeight;
  int xStart, xEnd;
  int col;

  col = y*width;

  if(x1 > x2){
    std::swap(x1, x2);
    std::swap(z1, z2);
    std::swap(w1, w2);
    std::swap(s1, s2);
    std::swap(t1, t2);
  }

  xStart = fpceil(x1);
  xEnd = fpceil(x2) - 65536;
  xError = xStart - x1;
  xStart >>= 16;
  xEnd >>= 16;

  deltaX = x2 - x1;
  deltaZ = z2 - z1;
  deltaW = w2 - w1;
  deltaS = s2 - s1;
  deltaT = t2 - t1;

  slopeZ = slopeW = slopeS = slopeT = 0;

  if(deltaX > 0){
    slopeZ = ((long long)deltaZ << 16) / deltaX;
    slopeW = ((long long)deltaW << 16) / deltaX;
    slopeS = ((long long)deltaS << 16) / deltaX;
    slopeT = ((long long)deltaT << 16) / deltaX;
  } else {
    return;
  }
  
  /* start interpolants */
  sStart = s1;
  tStart = t1;
  zStart = z1;
  wStart = w1;

  /* Correct for the new x position */
  zStart += ((long long)slopeZ * xError)>>16;
  wStart += ((long long)slopeW * xError)>>16;
  sStart += ((long long)slopeS * xError)>>16;
  tStart += ((long long)slopeT * xError)>>16;  

  zbuffer = &depthbuffer.data[col];
  const unsigned int* texture = &currentTexture->color[0];
  texWidth = currentTexture->width;
  texHeight = currentTexture->height;
  
  int indexDst = xStart + col;    
  int indexSrc;
  for(; xStart <= xEnd; ++xStart){
    z = zStart;
    if(z < zbuffer[xStart]){
      zbuffer[xStart] = z;
      w = 0x100000000LL / wStart;
      s = ((long long)w * sStart)  >> 16;
      t = ((long long)w * tStart)  >> 16;
      s = ((long long)s * (texWidth - 1));
      t = ((long long)t * (texHeight - 1));
      s >>= 16;
      t >>= 16;
      indexSrc = s + t*texWidth;
      cbuffer[indexDst] = texture[indexSrc];      
    }
    ++indexDst;
    zStart += slopeZ;
    wStart += slopeW;
    sStart += slopeS;
    tStart += slopeT;
  }
}

void DrawTriangle(std::vector<Vector4f>& vertexData,
		  std::vector<Vector4f>& textureData,
		  unsigned int* buffer,
		  unsigned int width,
		  unsigned int height
		  )
{
  //  const float eps = 0.00001f;
  for(int i=0; i<vertexData.size(); i+=3){
    Vector4f& v1 = vertexData[i+0];
    Vector4f& v2 = vertexData[i+1];
    Vector4f& v3 = vertexData[i+2];

    Vector4f& tc1 = textureData[i+0];
    Vector4f& tc2 = textureData[i+1];
    Vector4f& tc3 = textureData[i+2];


    /* deltas below are always positive due to this sorting. v1 = top, v2 = middle, v3 = bottom */
    if(v1.y > v2.y){
      std::swap(v1, v2);
      std::swap(tc1, tc2);
    }
    if(v2.y > v3.y){
      std::swap(v2, v3);
      std::swap(tc2, tc3);
    }
    if(v1.y > v2.y){
      std::swap(v1, v2);
      std::swap(tc1, tc2);
    }

    /* Q15.16 fixedpoint values*/
    Vector4i v1fp(v1.x * 65536.0f, v1.y * 65536.0f, v1.z * 65535.0f, v1.w * 65536.0f);
    Vector4i v2fp(v2.x * 65536.0f, v2.y * 65536.0f, v2.z * 65535.0f, v2.w * 65536.0f);
    Vector4i v3fp(v3.x * 65536.0f, v3.y * 65536.0f, v3.z * 65535.0f, v3.w * 65536.0f);

    Vector4i tc1fp(tc1.x * 65536.0f, tc1.y * 65536.0f, 0.0f, 0.0f);
    Vector4i tc2fp(tc2.x * 65536.0f, tc2.y * 65536.0f, 0.0f, 0.0f);
    Vector4i tc3fp(tc3.x * 65536.0f, tc3.y * 65536.0f, 0.0f, 0.0f);

    Vector4i delta1PTfp = v2fp - v1fp;
    Vector4i delta2PTfp = v3fp - v1fp;
    Vector4i delta3PTfp = v3fp - v2fp;

    Vector4i delta1TCfp = tc2fp - tc1fp;
    Vector4i delta2TCfp = tc3fp - tc1fp;
    Vector4i delta3TCfp = tc3fp - tc2fp;

    /* w not copied properly in operator- for Vector4<T>*/
    delta1PTfp.w = v2fp.w - v1fp.w;
    delta2PTfp.w = v3fp.w - v1fp.w;
    delta3PTfp.w = v3fp.w - v2fp.w;

    /* Slopes for x,z,w (vertices) and u,v (tcoords) */
    int slope1X, slope2X, slope3X;
    int slope1Z, slope2Z, slope3Z;
    int slope1W, slope2W, slope3W;
    int slope1S, slope2S, slope3S;
    int slope1T, slope2T, slope3T;

    slope1X = slope2X = slope3X = 0;
    slope1Z = slope2Z = slope3Z = 0;
    slope1W = slope2W = slope3W = 0;
    slope1S = slope2S = slope3S = 0;
    slope1T = slope2T = slope3T = 0;

    if(delta1PTfp.y > 0){
      slope1X = ((long long)delta1PTfp.x << 16) / delta1PTfp.y;
      slope1Z = ((long long)delta1PTfp.z << 16) / delta1PTfp.y;
      slope1W = ((long long)delta1PTfp.w << 16) / delta1PTfp.y;
      slope1S = ((long long)delta1TCfp.x << 16) / delta1PTfp.y;
      slope1T = ((long long)delta1TCfp.y << 16) / delta1PTfp.y;
    }

    if(delta2PTfp.y > 0){
      slope2X = ((long long)delta2PTfp.x << 16) / delta2PTfp.y;
      slope2Z = ((long long)delta2PTfp.z << 16) / delta2PTfp.y;
      slope2W = ((long long)delta2PTfp.w << 16) / delta2PTfp.y;
      slope2S = ((long long)delta2TCfp.x << 16) / delta2PTfp.y;
      slope2T = ((long long)delta2TCfp.y << 16) / delta2PTfp.y;
    }

    if(delta3PTfp.y > 0){
      slope3X = ((long long)delta3PTfp.x << 16) / delta3PTfp.y;
      slope3Z = ((long long)delta3PTfp.z << 16) / delta3PTfp.y;
      slope3W = ((long long)delta3PTfp.w << 16) / delta3PTfp.y;
      slope3S = ((long long)delta3TCfp.x << 16) / delta3PTfp.y;
      slope3T = ((long long)delta3TCfp.y << 16) / delta3PTfp.y;
    }

    int y1, y2;
    int x1, x2;
    int z1, z2;
    int w1, w2;
    int s1, s2;
    int t1, t2;
    int yError, yErrorInv;

    y1 = fpceil(v1fp.y);
    y2 = fpceil(v2fp.y) - 65536;
    yError = y1 - v1fp.y;
    yErrorInv = 65536 - yError;
    x1 = x2 = v1fp.x;
    z1 = z2 = v1fp.z;
    w1 = w2 = v1fp.w;
    s1 = s2 = tc1fp.x;
    t1 = t2 = tc1fp.y;

    /* Correct for the new y position */    
    x1 += ((long long)slope1X * yError) >> 16;
    x2 += ((long long)slope2X * yError) >> 16;
    z1 += ((long long)slope1Z * yError) >> 16;
    z2 += ((long long)slope2Z * yError) >> 16;
    w1 += ((long long)slope1W * yError) >> 16;
    w2 += ((long long)slope2W * yError) >> 16;
    s1 += ((long long)slope1S * yError) >> 16;
    s2 += ((long long)slope2S * yError) >> 16;
    t1 += ((long long)slope1T * yError) >> 16;
    t2 += ((long long)slope2T * yError) >> 16;

    y1 >>= 16;
    y2 >>= 16;
    /* Skipped if delta1f.y < 1 */
    for(; y1<=y2; ++y1){
      drawScanLine(buffer, width, height, y1, x1, x2, z1, z2, w1, w2, s1, s2, t1, t2);
      z1 += slope1Z;
      z2 += slope2Z;
      w1 += slope1W;
      w2 += slope2W;
      s1 += slope1S;
      s2 += slope2S;
      t1 += slope1T;
      t2 += slope2T;
      x1 += slope1X; /* middle - top */
      x2 += slope2X; /* bottom - top */
    }

    /* Next triangle part */
    y1 = fpceil(v2fp.y);
    y2 = fpceil(v3fp.y) - 65536;
    yError = y1 - v2fp.y;
    yErrorInv = 65536 - yError;
    x1 = v2fp.x;
    z1 = v2fp.z;
    w1 = v2fp.w;
    s1 = tc2fp.x;
    t1 = tc2fp.y;

    /* Interpolate to find this point*/
    x2 = v1fp.x;
    z2 = v1fp.z;
    w2 = v1fp.w;
    s2 = tc1fp.x;
    t2 = tc1fp.y;
    x2 += ((long long)slope2X * delta1PTfp.y) >> 16;
    z2 += ((long long)slope2Z * delta1PTfp.y) >> 16;
    w2 += ((long long)slope2W * delta1PTfp.y) >> 16;
    s2 += ((long long)slope2S * delta1PTfp.y) >> 16;
    t2 += ((long long)slope2T * delta1PTfp.y) >> 16;

    /* Correct for the new y position */
    x1 += ((long long)slope3X * yError) >> 16;
    x2 += ((long long)slope2X * yError) >> 16;
    z1 += ((long long)slope3Z * yError) >> 16;
    z2 += ((long long)slope2Z * yError) >> 16;
    w1 += ((long long)slope3W * yError) >> 16;
    w2 += ((long long)slope2W * yError) >> 16;
    s1 += ((long long)slope3S * yError) >> 16;
    s2 += ((long long)slope2S * yError) >> 16;
    t1 += ((long long)slope3T * yError) >> 16;
    t2 += ((long long)slope2T * yError) >> 16;
    
    y1 >>= 16;
    y2 >>= 16;
    /* Never iterated if delta3f.y < 1 */
    for(; y1<=y2; ++y1){
      drawScanLine(buffer, width, height, y1, x1, x2, z1, z2, w1, w2, s1, s2, t1, t2);
      z1 += slope3Z;
      z2 += slope2Z;
      w1 += slope3W;
      w2 += slope2W;
      s1 += slope3S;
      s2 += slope2S;
      t1 += slope3T;
      t2 += slope2T;
      x1 += slope3X; /* bottom - middle */
      x2 += slope2X; /* bottom - top */
    }
  }
}
