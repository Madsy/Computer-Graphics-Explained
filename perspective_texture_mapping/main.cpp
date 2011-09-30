#include <SDL/SDL.h>
#include <vector>
#include <algorithm>
#include <linealg.h>
#include "clipplane.h"
#include "framebuffer.h"
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
    std::vector<Vector4f> workingCopyVertex;  /* Intermediate working copy */ 
    std::vector<Vector4f> workingCopyTCoord;  /* Intermediate working copy */ 
    std::vector<unsigned int> texBuf;   /* RGBA Texture image */

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(width, height, depth, SDL_DOUBLEBUF | SDL_SWSURFACE);
    SDL_WM_SetCaption("MechCore.net Projection Example", NULL);
    
    makeMeshCube(vertexData, tcoordData, 1.0f);
    const Texture* texture = ReadPNG("texture0.png");
    if(!texture){
	printf("Couldn't load one or more texture maps.\n \
Make sure you have copied the data from the source directory to the binary directory, or CWD.\n");
	return -1;
    }
    BindTexture(texture);
    /* Initialize our buffers */
    InitBuffers(width, height);

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

        float time_elapsed = (float)SDL_GetTicks() * 0.001f;
        /* We need a new working copy every frame */
        workingCopyVertex.resize(vertexData.size());
	workingCopyTCoord.resize(tcoordData.size());
	/* Copy our original data into the working copies */
        std::copy(vertexData.begin(), vertexData.end(), workingCopyVertex.begin());
        std::copy(tcoordData.begin(), tcoordData.end(), workingCopyTCoord.begin());
	/* Make sure our buffers are of the same size */
	ASSERT(workingCopyVertex.size() == workingCopyTCoord.size());

        /* world matrix transform */
        Matrix4f worldMatrix = translate(Vector4f(0.0f, 0.0f, -2.25f, 1.0f))  *
	rotateY(45.0f *   time_elapsed);
	
	/* perspective function is in linealg.h under /include */
        Matrix4f clipMatrix = perspective(90.0f, 4.0f/3.0f, 0.01f, 20.0f);
	Matrix4f worldClipMatrix = clipMatrix * worldMatrix;
	
        /* Transform our points */
	for(unsigned int i=0; i<workingCopyVertex.size(); i+=3){
	    Vector4f& p1 = workingCopyVertex[i];
	    Vector4f& p2 = workingCopyVertex[i+1];
	    Vector4f& p3 = workingCopyVertex[i+2];
            p1 = worldClipMatrix * p1;
	    p2 = worldClipMatrix * p2;
	    p3 = worldClipMatrix * p3;
	}

	/* Clip against the six frustum planes */
#if 0
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f(-1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  1.0f, 0.0f, 1.0f));
        clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f, -1.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  0.0f,-1.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  0.0f, 1.0f, 1.0f));
#endif	
	ASSERT(workingCopyVertex.size() == workingCopyTCoord.size());
	/* Assert that we have whole triangles after clipping */
	ASSERT(!(workingCopyVertex.size() % 3));

	for(unsigned int i=0; i<workingCopyVertex.size(); ++i)
	{
	    /* does not divide w by w */
	    //workingCopyVertex[i] /= workingCopyVertex[i].w;
	    float wInv = 1.0f / workingCopyVertex[i].w;
	    workingCopyVertex[i].w = wInv;
	    workingCopyVertex[i].x *= wInv;
	    workingCopyVertex[i].y *= wInv;
	    workingCopyVertex[i].z *= wInv;
	    workingCopyTCoord[i].x *= wInv;
	    workingCopyTCoord[i].y *= wInv;

	    /* project function is in linealg.h under /include
	       x and y is in screenspace
	       z is normalized into [0,1> range
	       w = 1.0 / w
	    */
	    workingCopyVertex[i] = project(workingCopyVertex[i], (float)width, (float)height);
	    /* Store as fixedpoint. We want to interpolate 1/w across the edges.
	       The interpolated 1/w is flipped again, that is w = 1.0 / (v0.w + t*(v1.w - v0.w))
	       z is stored in the range [0.0, 1.0] inclusive. That is, 65535 = 1.0 */
	}
	
        unsigned int* pixels = static_cast<unsigned int*>(screen->pixels);
	/* Clear our depth buffer */
	ClearBuffer(DEPTH_BUFFER);
        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
	/* Draw the triangles */
	DrawTriangle(workingCopyVertex, workingCopyTCoord, pixels, width, height);

	SDL_Flip(screen);
    }    
    SDL_Quit();
    return 0;
}

/*
z' = 1 / (1/z)
u = (u/z) * z
v = (v/z) * z 
*/
