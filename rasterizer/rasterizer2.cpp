#include <linealg.h>
#include <fixedpoint.h>
#include "mesh.h"

void Tri2Split(std::list<Triangle>& triangle)
{
    using std::cout;
    using std::endl;
    
    std::list<Triangle>::iterator tr = triangle.begin();
    
    while(tr != triangle.end())
    {
        Vertex& v0 = tr->v0;
        Vertex& v1 = tr->v1;
        Vertex& v2 = tr->v2;
        
        if(v0.point.y > v1.point.y)
            std::swap(v0, v1);
        if(v1.point.y > v2.point.y)
            std::swap(v1, v2);
        if(v0.point.y > v1.point.y)
            std::swap(v0, v1);
        
        /* don't split if we already have a horisontal edge */
        if((v0.point.y == v1.point.y) || (v1.point.y == v2.point.y)){
            tr++;
            continue;
        }
        
	/* Check for a degenerate triangle */
        if( (v0.point.y == v1.point.y) && (v1.point.y == v2.point.y) ){
            tr = triangle.erase(tr);
            continue;
        }
        /* Split in two triangles */

        Vector2 edge0 = v1.point - v0.point;
        Vector2 edge1 = v2.point - v0.point;
        TCoord tcoord_edge = v2.tcoord - v0.tcoord;
        
        ASSERT(edge0.y);
        
        long point_slope =  ((long long)edge1.x << 16) / edge1.y;
        
        long tcoord_s_slope = ((long long)tcoord_edge.s << 16) / edge1.y;
        long tcoord_t_slope = ((long long)tcoord_edge.t << 16) / edge1.y;
        
        //new point
        Vector2 point( v0.point.x  + (((long long)edge0.y * point_slope) >> 16), v1.point.y, 1<<16);
        //new tcoord
        TCoord tcoord( v0.tcoord.s + (((long long)edge0.y * tcoord_s_slope) >> 16),
                       v0.tcoord.t + (((long long)edge0.y * tcoord_t_slope) >> 16) );

        Vertex v3(point, tcoord);
        
        triangle.push_front( Triangle(v1, v3, v2) );
        triangle.push_front( Triangle(v0, v1, v3) );
        tr = triangle.erase(tr);
    }
}


void Render(const std::list<Trianglef>& triangle)
{
    Vector2 edge0, edge1;
    std::list<Trianglef>::const_iterator it;
    /* fixedpoint */
    long slope0, slope1, x0, x1, y0, y1;
    long yError;
    Vector2i p0,p1,p2;
    Vector2i  t0,t1,t2;
    
    Vector2i delta_tcoord0, delta_tcoord1, slope_tcoord0, slope_tcoord1;
    Vector2i  tcoord_start, tcoord_end; //traversal with added slopes
    
    for(it=triangle.begin(); it!=triangle.end(); it++){
        
        const Vertexf* v0 = &it->v0;
        const Vertexf* v1 = &it->v1;
        const Vertexf* v2 = &it->v2;
    
        /* p0 and p1 is on top */        
        if(v0->point.y == v1->point.y){
            if(v1->point.x < v0->point.x)
                std::swap(v0, v1);
            
	    /* convert from float to Q11.4 fixedpoint */
            p0.x = v0->point.x * 16.0f;
	    p0.y = v0->point.y * 16.0f;
	    p0.z = v0->point.z * 16.0f;
            p1.x = v1->point.x * 16.0f;
	    p1.y = v1->point.y * 16.0f;
	    p1.z = v1->point.z * 16.0f;
            p2.x = v2->point.x * 16.0f;
	    p2.y = v2->point.y * 16.0f;
	    p2.z = v2->point.z * 16.0f;
            
            t0.x = v0->tcoord.x * 16.0f;
	    t0.y = v0->tcoord.y * 16.0f;
            t1.x = v1->tcoord.x * 16.0f;
	    t1.y = v1->tcoord.y * 16.0f;
	    t2.x = v2->tcoord.x * 16.0f;
	    t2.y = v2->tcoord.y * 16.0f;
            
            x0 = p0.x;
            x1 = p1.x;
            y0 = ceilfp(p0.y);
            y1 = ceilfp(p2.y) - (1<<4);
            
            tcoord_start = t0;
            tcoord_end   = t1;
            
            edge0 = p2 - p0;
            edge1 = p2 - p1;
            
            delta_tcoord0 = t2 - t0;
            delta_tcoord1 = t2 - t1;
        }
        else { /* p0 is on top, p1 and p2 is below */
            if(v2->point.x < v1->point.x)
                std::swap(v1, v2);
            
            /* convert from float to Q11.4 fixedpoint */
            p0.x = v0->point.x * 16.0f;
	    p0.y = v0->point.y * 16.0f;
	    p0.z = v0->point.z * 16.0f;
            p1.x = v1->point.x * 16.0f;
	    p1.y = v1->point.y * 16.0f;
	    p1.z = v1->point.z * 16.0f;
            p2.x = v2->point.x * 16.0f;
	    p2.y = v2->point.y * 16.0f;
	    p2.z = v2->point.z * 16.0f;
            
            t0.x = v0->tcoord.x * 16.0f;
	    t0.y = v0->tcoord.y * 16.0f;
            t1.x = v1->tcoord.x * 16.0f;
	    t1.y = v1->tcoord.y * 16.0f;
	    t2.x = v2->tcoord.x * 16.0f;
	    t2.y = v2->tcoord.y * 16.0f;
            
            x0 = p0.x;
            x1 = p0.x;
            y0 = ceilfp(p0.y);
            y1 = ceilfp(p1.y) - (1<<4);
            
            tcoord_start = t0;
            tcoord_end   = t0;
            
            edge0 = p1 - p0;
            edge1 = p2 - p0;
            
            delta_tcoord0 = t1 - t0;
            delta_tcoord1 = t2 - t0;
        }
        
        /* slope for leftmost edge */
        if(!edge0.y){
            slope0 = 0;
            slope_tcoord0 = Vector2i(0,0);
        }
        else{
            slope0          = (edge0.x        <<4) / edge0.y;
            slope_tcoord0.x = (delta_tcoord0.x<<4) / edge0.y;
            slope_tcoord0.y = (delta_tcoord0.y<<4) / edge0.y;
        }
        
        /* slope for rightmost edge */
        if(!edge1.y){
            slope1 = 0;
            slope_tcoord1 = Vector2i(0,0);
        }
        else{
            slope1          = (edge1.x        <<4) / edge1.y;
            slope_tcoord1.x = (delta_tcoord1.x<<4) / edge1.y;
            slope_tcoord1.y = (delta_tcoord1.y<<4) / edge1.y;
        }
        
        yError = y0 - p0.y;
        
        /* correct for offseted start y-coord */
        if(yError)
        {
            x0 += (slope0*yError) >> 4;
            x1 += (slope1*yError) >> 4;
        
            tcoord_start.s += ((long long)slope_tcoord0.s*yError) >> 16;
            tcoord_start.t += ((long long)slope_tcoord0.t*yError) >> 16;
            
            tcoord_end.s   += ((long long)slope_tcoord1.s*yError) >> 16;
            tcoord_end.t   += ((long long)slope_tcoord1.t*yError) >> 16;
        }
        
        y0 >>= 16;
        y1 >>= 16;
        
        while(y0 <= y1)
        {
	    long PosX = ceilfp(x0);
	    long EndX = ceilfp(x1) - (1<<16);
	    TCoord PosTex = tcoord_start;
	    TCoord SlopeTex;
	    long xError = PosX - x0;
	    long xDelta = x1 - x0;
	    long column = y0*Width;
                
	    if(!xDelta){
		SlopeTex = TCoord(0,0);
	    }
	    else {
		SlopeTex.s = ((long long)(tcoord_end.s - tcoord_start.s) << 16) / xDelta;
		SlopeTex.t = ((long long)(tcoord_end.t - tcoord_start.t) << 16) / xDelta;
	    }
                
	    //Correct for offseted x coord
	    //checked for positive xDelta, or else the slopes are 0 and can be ignored
	    if(xError && xDelta){
		PosTex.s   += ((long long)SlopeTex.s * xError) >> 16;
		PosTex.t   += ((long long)SlopeTex.t * xError) >> 16;
	    }
                
	    PosX >>= 16;
	    EndX >>= 16;

	    for(;PosX <= EndX; ++PosX){
		unsigned long s = ((long long)PosTex.s * (texWidth-1)) >> 16;
		unsigned long t = ((long long)PosTex.t * (texHeight-1)) >> 16;
		unsigned long texel = texture[s + t*texWidth];
                    
		pixel[PosX + column] = texel;
                    
		PosTex.s   += SlopeTex.s;
		PosTex.t   += SlopeTex.t;
	    }
         
            x0 += slope0;
            x1 += slope1;
            tcoord_start.s += slope_tcoord0.s;
            tcoord_start.t += slope_tcoord0.t;
            tcoord_end.s   += slope_tcoord1.s;
            tcoord_end.t   += slope_tcoord1.t;
            ++y0;
        }
    }
}
