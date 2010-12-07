#include <SDL/SDL.h>
#include <vector>
#include <algorithm>
#include <linealg.h>
#include "clipplane.h"
#include "line.h"
#include "meshgen.h"
#include "myassert.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
	
    const int width = 640;
    const int height = 480;
    const int depth = 32;
    bool running = true;
    SDL_Event event;
    std::vector<Vector4f> triangleMesh;
    std::vector<Vector4f> workingCopy;   
 
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* screen = SDL_SetVideoMode(width, height, depth, SDL_DOUBLEBUF | SDL_HWSURFACE);
    SDL_WM_SetCaption("MechCore.net Projection Example", NULL);
    
    makeMeshSphere(triangleMesh, 2.0f);
 
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

        float time = (float)SDL_GetTicks() * 0.001f;
        /* We need a new working copy every frame */
        workingCopy.resize(triangleMesh.size());
        std::copy(triangleMesh.begin(), triangleMesh.end(), workingCopy.begin());
	/* changing translate in the x-axis, to test clipping */
	float xtrans = 3.7f * std::sin(PI * 2.0f * time * 0.125f);
	xtrans = -xtrans;
	//float xtrans = 3.7f;
	//printf("x translation: %f\n", xtrans);
        /* world matrix transform */
        Matrix4f worldMatrix = translate(Vector4f(xtrans, 0.0f, -3.25f, 1.0f)); /* * 
								rotateY(time * 90.0f);*/ /*  *
								rotateX(time * 45.0f) *
								rotateZ(time * 22.5f);   */

	/* perspective function is in linealg.h under /include */
        Matrix4f clipMatrix = perspective(90.0f, 4.0f/3.0f, 0.01f, 20.0f);
	Matrix4f worldClipMatrix = clipMatrix * worldMatrix;
				
        /* Transform our points */

        for(unsigned int i=0; i<workingCopy.size(); i+=3){
	    Vector4f& p1 = workingCopy[i];
	    Vector4f& p2 = workingCopy[i+1];
	    Vector4f& p3 = workingCopy[i+2];

            p1 = worldClipMatrix * p1;
	    p2 = worldClipMatrix * p2;
	    p3 = worldClipMatrix * p3;
	}

	clip_triangle(workingCopy, Vector4f(-1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopy, Vector4f( 1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopy, Vector4f( 0.0f,  1.0f, 0.0f, 1.0f));
        clip_triangle(workingCopy, Vector4f( 0.0f, -1.0f, 0.0f, 1.0f));

	ASSERT(!(workingCopy.size() % 3));
	for(unsigned int i=0; i<workingCopy.size(); ++i)
	{
	    /* does not divide w by w */
	    workingCopy[i] /= workingCopy[i].w;
	    /* z should be in range of [0, 1] */
	    workingCopy[i].z = workingCopy[i].z * 0.5f + 0.5f;
	    /* project to screenspace
	       project function is in linealg.h under /include */
	    workingCopy[i] = project(workingCopy[i], (float)width, (float)height);
	}

        SDL_LockSurface(screen);
        Uint32* pixels = static_cast<Uint32*>(screen->pixels);

        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);

        for(unsigned int i=0; i<workingCopy.size(); i+=3){

	    Vector4f& p1 = workingCopy[i]; 
	    Vector4f& p2 = workingCopy[i+1]; 
	    Vector4f& p3 = workingCopy[i+2]; 

	    drawLine(p1, p2, pixels, width, height);
	    drawLine(p2, p3, pixels, width, height);
	    drawLine(p3, p1, pixels, width, height);
        }
	SDL_Flip(screen);
        SDL_UnlockSurface(screen);
    }    
    SDL_Quit();
    return 0;
}
