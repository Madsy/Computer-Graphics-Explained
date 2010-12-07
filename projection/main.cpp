#include <vector>
#include <SDL/SDL.h>
#include <linealg.h>
#include "line.h"
#include "meshgen.h"

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
    
    makeMeshSphere(points, 2.0f);
 
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
				
        for(unsigned int i=0; i<workingCopy.size(); ++i){
			/* Transform our points into clip space*/
			Vector4f& p = workingCopy[i];
            p = worldClipMatrix * p;
			/****************** Add clipping here ***************
			******************************************************/
			/* After clipping we do the 'perspective divide'. The coordinates are now known as
			   normalized device coordinates. */
            p /= p.w;
            p.z = 0.5f*p.z + 0.5f;
        }

        SDL_LockSurface(screen);
        Uint32* pixels = static_cast<Uint32*>(screen->pixels);
        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
        for(unsigned int i=0; i<workingCopy.size(); i+=3){
	    /* This projects our 4D NDCs to 2D screen space,
				where pixels are the units */
            Vector4f p1 = project(workingCopy[i], (float)width, (float)height);
            Vector4f p2 = project(workingCopy[i+1], (float)width, (float)height);
	    Vector4f p3 = project(workingCopy[i+2], (float)width, (float)height);

            /* Draw the sphere white line segments (wireframe) if it is inside the viewport bounds */
            drawLine(p1, p2, pixels, width, height);
	    drawLine(p2, p3, pixels, width, height);
	    drawLine(p3, p1, pixels, width, height);
        }
        SDL_UnlockSurface(screen);
        SDL_Flip(screen);
    }    
    SDL_Quit();

    return 0;
}

