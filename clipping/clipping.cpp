#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include "../include/linealg.h"

static void makeSpherePointCloudMesh(std::vector<Vector4f>& dst, float radius)
{

    const int resolution = 16;
    const float halfPI = PI * 0.5f;
    float interp = 1.0f / (float)resolution;
    Vector4f v0,v1,v2,v3;
    radius *= 0.5f;
    
    for(int i=0; i<=resolution; ++i){
        float theta0 = interp*(float)(i+0)*PI - halfPI;
        float theta1 = interp*(float)(i+1)*PI - halfPI;
        float z1 = std::sin(theta0);
        float z2 = std::sin(theta1);
        
        v0.z = z1 * radius;
        v1.z = z1 * radius;
        v2.z = z2 * radius;
        v3.z = z2 * radius;
        
        for(int j=0; j<=resolution; ++j){
            float phi0 = interp*(float)(j+0)*2.0f*PI;
            float phi1 = interp*(float)(j+1)*2.0f*PI;
            float x1 = std::cos(theta0)*std::cos(phi0);
			float x2 = std::cos(theta0)*std::cos(phi1);
            float y1 = std::cos(theta0)*std::sin(phi0);
            float y2 = std::cos(theta0)*std::sin(phi1);
			float x3 = std::cos(theta1)*std::cos(phi0);
			float x4 = std::cos(theta1)*std::cos(phi1);
            float y3 = std::cos(theta1)*std::sin(phi0);
            float y4 = std::cos(theta1)*std::sin(phi1);
            v0.x = x1 * radius;
            v0.y = y1 * radius;
            v1.x = x2 * radius;
            v1.y = y2 * radius;
            v2.x = x3 * radius;
            v2.y = y3 * radius;
            v3.x = x4 * radius;
            v3.y = y4 * radius;
            
            dst.push_back(v0); dst.push_back(v1);
            dst.push_back(v1); dst.push_back(v2);
            dst.push_back(v2); dst.push_back(v3);
            dst.push_back(v3); dst.push_back(v0);
        }
    }
}

static void drawLineYMajor(const Vector4f& v1, const Vector4f& v2,
					unsigned int* buffer, int width, int height)
{		
	Vector2i p1(v1.x, v1.y);
	Vector2i p2(v2.x, v2.y);
	
	if(p2.y < p1.y)
		std::swap(p1,p2);
	Vector2i edge = p2-p1;

	int xslope = (edge.x<<16) / edge.y;
	int sign = (xslope < 0) ? -1 : 1;
	int xaccum = 0;
	int x=p1.x;
	
	xslope = std::abs(xslope);
	for(int y=p1.y; y<=p2.y; ++y){
		xaccum += xslope;
		if(xaccum >= 65536){
			xaccum -= 65536;
			x+=sign;
		}
		buffer[x+y*width] = 0xFFFFFFFF;
	}
}

static void drawLineXMajor(const Vector4f& v1, const Vector4f& v2,
					unsigned int* buffer, int width, int height)
{
	Vector2i p1(v1.x, v1.y);
	Vector2i p2(v2.x, v2.y);
	if(p2.x < p1.x)
		std::swap(p1, p2);
	Vector2i edge = p2-p1;
		
	int yslope = (edge.y<<16) / edge.x;
	int sign = (yslope < 0) ? -1 : 1;
	int yaccum = 0;
	int y=p1.y;
	
	yslope = std::abs(yslope);
	for(int x=p1.x; x<=p2.x; ++x){
		yaccum += yslope;
		if(yaccum >= 65536){
			yaccum -= 65536;
			y+=sign;
		}
		buffer[x+y*width] = 0xFFFFFFFF;
	}
}

static void drawLineVertical(const Vector4f& v1, const Vector4f& v2,
					unsigned int* buffer, int width, int height)
{
	int x = v1.x;
	int y1 = v1.y;
	int y2 = v2.y;
	
	for(int y=v1.y; y<v2.y; ++y)
		buffer[x + y*width] = 0xFFFFFFFF;
}
static void drawLineHorisontal(const Vector4f& v1, const Vector4f& v2,
					unsigned int* buffer, int width, int height)
{
	int y = v1.y;
	int x1 = v1.x;
	int x2 = v2.x;
	
	for(int x=v1.x; x<v2.x; ++x)
		buffer[x + y*width] = 0xFFFFFFFF;
}

static void drawLine(const Vector4f& v1, const Vector4f& v2,
					unsigned int* buffer, int width, int height)
{
	Vector2i p1(v1.x, v1.y);
	Vector2i p2(v2.x, v2.y);
	
	if(p2.y < p1.y)
		std::swap(p1, p2);
	
	/* Points downwards, sign dependent on x */	
	Vector2i edge = p2 - p1;
	
	if(!edge.x && !edge.y)
		return;
		
	if(!edge.y){
		if(p2.x < p1.x)
			std::swap(p1, p2);
		drawLineHorisontal(v1, v2, buffer, width, height);
	} else if(!edge.x){
		drawLineVertical(v1, v2, buffer, width, height);
	}  else {
		/* sloped line */
		
		/* y always positive, sign depends on x */
		int slope = (edge.x<<16) / edge.y;
		if(std::abs(slope) <= 65536){
			drawLineYMajor(v1, v2, buffer, width, height);
		} else {
		   /* x major */
			drawLineXMajor(v1, v2, buffer, width, height);
		}
	}
	return;
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	
    const int width = 640;
    const int height = 480;
    const int depth = 32;
    bool running = true;
    SDL_Event event;
    std::vector<Vector4f> points;
    std::vector<Vector4f> workingCopy;   
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* screen = SDL_SetVideoMode(width, height, depth, SDL_DOUBLEBUF | SDL_HWSURFACE);
	SDL_WM_SetCaption("MechCore.net Projection Example", NULL);
    
    makeSpherePointCloudMesh(points, 2.0f);
 
    while(running){
        while(SDL_PollEvent(&event)){
            switch(event.type)
            {
            case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE){
					running = false;
				}
				break;
            case SDL_QUIT:
                running = false;
                break;
            }
        }

        /* This is the code that matters. Every frame we make a fresh copy of our untransformed mesh,
           rotate and translate it, and project it. Finally we render it, if the point(s) is inside
           the frustum */
           
        /* Animation based on time, not how fast we render */
        float time = (float)SDL_GetTicks() * 0.001f;
        /* We need a new working copy every frame */
        workingCopy.resize(points.size());
        std::copy(points.begin(), points.end(), workingCopy.begin());

        Matrix4f worldMatrix = translate(Vector4f(0.0f, 0.0f, -3.05f, 1.0f)) * 
								rotateY(time * 90.0f); /*  *
								rotateX(time * 45.0f) *
								rotateZ(time * 22.5f);   */
								
        Matrix4f clipMatrix = perspective(90.0f, 4.0f/3.0f, 0.01f, 20.0f);
		Matrix4f worldClipMatrix = clipMatrix * worldMatrix;
				
        /* Transform our points */
        for(unsigned int i=0; i<workingCopy.size(); ++i){
            workingCopy[i] = worldClipMatrix * workingCopy[i];
            Vector4f& p = workingCopy[i];
            float zn = p.z / p.w;
            zn = 0.5f*zn + 0.5f;

            /* Perspective divide. The coordinates are now known as
               normalized device coordinates. Clipping is done in this space.
               Clipping is explained on the wiki, and in another example. */
            workingCopy[i] /= workingCopy[i].w;
        }
        
        /* Draw order should be based on depth
			(not very important when drawing points though)
			Depth testing is explained in greater detail on the wiki,
			and in another code example. You don't need to understand it yet.*/
        //std::sort(workingCopy.begin(), workingCopy.end());

        SDL_LockSurface(screen);
        Uint32* pixels = static_cast<Uint32*>(screen->pixels);
        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
        for(unsigned int i=1; i<workingCopy.size(); i+=2){
			/* This projects our 4D NDCs to 2D screen space,
				where pixels are the units */
            Vector4f p1 = project(workingCopy[i-1], (float)width, (float)height);
            Vector4f p2 = project(workingCopy[i], (float)width, (float)height);

            /* Draw the points as white pixels if it is inside the viewport bounds */
            drawLine(p1, p2, pixels, width, height);
            int x1 = p1.x;
            int y1 = p1.y;
            int x2 = p2.x;
            int y2 = p2.y;
            pixels[x1+y1*width] = 0xFFFF0000;
            pixels[x2+y2*width] = 0xFFFF0000;
        }
        SDL_UnlockSurface(screen);
        SDL_Flip(screen);
    }    
    SDL_Quit();

    return 0;
}
