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
    std::vector<Vector4f> workingCopyVertex;  /* Intermediate working copy */ 
    std::vector<Vector4f> workingCopyTCoord;  /* Intermediate working copy */ 
    std::vector<Vector4i> vertexDataFP;    /* Final copy, fixedpoint */
    std::vector<Vector4i> tcoordDataFP;    /* Final copy, fixedpoint */
    std::vector<unsigned int> texBuf;   /* RGBA Texture image */

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* screen = SDL_SetVideoMode(width, height, depth, SDL_DOUBLEBUF | SDL_SWSURFACE);
    SDL_WM_SetCaption("MechCore.net Projection Example", NULL);
    
    makeMeshPlane(vertexData, tcoordData, 1.0f);
    const Texture* texture = ReadPNG("texture0.png");
    if(!texture){
	printf("Couldn't load one or more texture maps.\n \
Make sure you have copied the data from the source directory to the binary directory, or CWD.\n");
    }
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
        Matrix4f worldMatrix = translate(Vector4f(0.0f, 0.0f, -2.25f, 1.0f)) *
	    rotateZ(11.175f * time_elapsed) *
	    rotateX(22.5f *   time_elapsed) *
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
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f(-1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 1.0f,  0.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  1.0f, 0.0f, 1.0f));
        clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f, -1.0f, 0.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  0.0f,-1.0f, 1.0f));
	clip_triangle(workingCopyVertex, workingCopyTCoord, Vector4f( 0.0f,  0.0f, 1.0f, 1.0f));
	
	ASSERT(workingCopyVertex.size() == workingCopyTCoord.size());
	/* Assert that we have whole triangles after clipping */
	ASSERT(!(workingCopyVertex.size() % 3));

	vertexDataFP.resize(workingCopyVertex.size());
	tcoordDataFP.resize(workingCopyTCoord.size());

	for(unsigned int i=0; i<workingCopyVertex.size(); ++i)
	{
	    /* does not divide w by w */
	    workingCopyVertex[i] /= workingCopyVertex[i].w;
	    /* z should be in range of [0, 1] */
	    workingCopyVertex[i].z = workingCopyVertex[i].z * 0.5f + 0.5f;
	    /* project to screenspace
	       project function is in linealg.h under /include */
	    workingCopyVertex[i] = project(workingCopyVertex[i], (float)width, (float)height);
	    /* Store as fixedpoint. We want to interpolate 1/w across the edges.
	       The interpolated 1/w is flipped again, that is w = 1.0 / (v0.w + t*(v1.w - v0.w)) */
	    vertexDataFP[i] = Vector4i(
				    workingCopyVertex[i].x * 65536.0f,
				    workingCopyVertex[i].y * 65536.0f,
				    workingCopyVertex[i].z * 65536.0f,
				    workingCopyVertex[i].w * 65536.0f
				    );
	    //ASSERT(workingCopyVertex[i].w != 0);
	    //ASSERT(VertexDataFP[i].w != 0);
	    tcoordDataFP[i] = Vector4i(workingCopyTCoord[i].x * 65536.0f,
				       workingCopyTCoord[i].y * 65536.0f,
				       0,
				       0
				       );
	}
	
        unsigned int* pixels = static_cast<unsigned int*>(screen->pixels);

        /* clear the screen to black */
        memset(pixels, 0, sizeof(Uint32) * width * height);
	/* Split the  triangles so that they start and end on horisontal edges*/
	TriangleSplit(vertexDataFP, tcoordDataFP);
	/* Draw the triangles */
	DrawTriangle(vertexDataFP, tcoordDataFP, pixels, width, height);

	SDL_Flip(screen);
    }    
    SDL_Quit();
    return 0;
}
