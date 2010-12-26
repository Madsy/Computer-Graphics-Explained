#include <SDL/SDL.h>
#include <vector>
#include <algorithm>
#include <linealg.h>
#include "clipplane.h"
#include "rasterizer.h"
#include "meshgen.h"
#include "texture.h"
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
    std::vector<Vector4f> vertexData; /* Our original mesh */
    std::vector<Vector4f> tcoordData; /* Our original mesh */
    std::vector<Vector4f> workingCopy;  /* Intermediate working copy */ 
    std::vector<Vector4i> vertexDataFP;    /* Final copy, fixedpoint */
    std::vector<Vector4i> tcoordDataFP;    /* Final copy, fixedpoint */
    std::vector<unsigned int> texBuf;   /* RGBA Texture image */

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* screen = SDL_SetVideoMode(width, height, depth, SDL_DOUBLEBUF | SDL_SWSURFACE);
    SDL_WM_SetCaption("MechCore.net Projection Example", NULL);
    
    makeMeshCube(vertexData, tcoordData, 1.0f);
    const Texture* texture = ReadPNG("texture0.png");
    ASSERT(texture != NULL);
    printf("Texture width: %d height %d\n", texture->width, texture->height);
    BindTexture(texture);
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
        workingCopy.resize(vertexData.size());
        std::copy(vertexData.begin(), vertexData.end(), workingCopy.begin());
       
        /* world matrix transform */
        Matrix4f worldMatrix = translate(Vector4f(0.0f, 0.0f, -3.25f, 1.0f)) *
	    rotateZ(11.175f * time) *
	    rotateX(22.5f * time) *
	    rotateZ(45.0f * time);
 
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

	/*
	clip_triangle(workingCopy, Vector4f(-1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopy, Vector4f( 1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopy, Vector4f( 0.0f,  1.0f, 0.0f, 1.0f));
        clip_triangle(workingCopy, Vector4f( 0.0f, -1.0f, 0.0f, 1.0f));
	*/

	ASSERT(!(workingCopy.size() % 3));
	ASSERT(workingCopy.size() == tcoordData.size());

	vertexDataFP.resize(workingCopy.size());
	tcoordDataFP.resize(workingCopy.size());

	for(unsigned int i=0; i<workingCopy.size(); ++i)
	{
	    /* does not divide w by w */
	    workingCopy[i] /= workingCopy[i].w;
	    /* z should be in range of [0, 1] */
	    workingCopy[i].z = workingCopy[i].z * 0.5f + 0.5f;
	    /* project to screenspace
	       project function is in linealg.h under /include */
	    workingCopy[i] = project(workingCopy[i], (float)width, (float)height);
	    /* Store as fixedpoint. We want to interpolate 1/w across the edges.
	       The interpolated 1/w is flipped again, that is w = 1.0 / (v0.w + t*(v1.w - v0.w)) */
	    vertexDataFP[i] = Vector4i(
				    workingCopy[i].x * 65536.0f,
				    workingCopy[i].y * 65536.0f,
				    workingCopy[i].z * 65536.0f,
				    (1.0f / workingCopy[i].w) * 65536.0f
				    );

	    ASSERT(tcoordData[i].x >= 0.0 && tcoordData[i].x <= 1.0f);
	    ASSERT(tcoordData[i].y >= 0.0 && tcoordData[i].y <= 1.0f);

	    tcoordDataFP[i] = Vector4i(tcoordData[i].x * 65536.0f,
				       tcoordData[i].y * 65536.0f,
				       0,
				       0
				       );
	}

        //SDL_LockSurface(screen);
        unsigned int* pixels = static_cast<unsigned int*>(screen->pixels);

        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
	/* draw the triangles */
	TriangleSplit(vertexDataFP, tcoordDataFP);
	DrawTriangle(vertexDataFP, tcoordDataFP, pixels, width, height);

	SDL_Flip(screen);
	//exit(0);
        //SDL_UnlockSurface(screen);
    }    
    SDL_Quit();
    return 0;
}
