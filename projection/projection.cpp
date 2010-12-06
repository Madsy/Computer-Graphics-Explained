#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include "../include/linealg.h"

static void makeSpherePointCloudMesh(std::vector<Vector4f>& dst, float radius)
{

    const int resolution = 16;
    const float halfPI = PI * 0.5f;
    float interp = 1.0f / (float)resolution;
    Vector4f v;
    v.w = 1.0f;
    
    for(int i=0; i<=resolution; ++i){
        float theta = interp*(float)i*PI - halfPI;
        v.z = std::sin(theta);
        for(int j=0; j<=resolution; ++j){
            float phi = interp*(float)j*2.0f*PI;
            v.x = std::cos(theta)*std::cos(phi);
            v.y = std::cos(theta)*std::sin(phi);
			Vector3f vn = v;
			vn = vn.unit() * radius;
            dst.push_back(Vector4f(vn, 1.0f));
        }
    }
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
    makeSpherePointCloudMesh(points, 1.0f);
    
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

        Matrix4f worldMatrix = translate(Vector4f(0.0f, 0.0f, -2.000000001f, 1.0f)) * rotateY(time * 90.0f);
        Matrix4f clipMatrix = perspective(90.0f, 4.0f/3.0f, 1.0f, 20.0f);
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
        std::sort(workingCopy.begin(), workingCopy.end());

        SDL_LockSurface(screen);
        Uint32* pixels = static_cast<Uint32*>(screen->pixels);
        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
        for(unsigned int i=0; i<workingCopy.size(); ++i){
			/* This projects our 4D NDCs to 2D screen space,
				where pixels are the units */
            Vector4f p = project(workingCopy[i], (float)width, (float)height);
            int x = (int)p.x;
            int y = (int)p.y;
            /* Draw the points as white pixels if it is inside the viewport bounds */
            if(x >= 0 && x < width
				&& y >= 0 && y < height
				&& p.z > -p.w && -p.w <= p.z && p.z <= p.w)
               pixels[x + y * width] = 0xFFFFFFFF;
        }
        SDL_UnlockSurface(screen);
        SDL_Flip(screen);
    }    
    SDL_Quit();

    return 0;
}

//printf("%2.3f, %2.3f, %2.3f, %2.3f %2.3f\n", p.x, p.y, p.z, p.w, zn);