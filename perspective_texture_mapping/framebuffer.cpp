#include <algorithm>
#include "framebuffer.h"

Buffer2D<unsigned int> colorbuffer;
Buffer2D<unsigned short> depthbuffer;
SDL_Surface* screen;

void InitBuffers(unsigned int width, unsigned int height)
{
    colorbuffer = Buffer2D<unsigned int>(width, height);
    depthbuffer = Buffer2D<unsigned short>(width, height);
    return;
}

void ClearBuffer(BufferType type)
{
    switch(type)
    {
    case COLOR_BUFFER:
	std::fill(colorbuffer.data.begin(), colorbuffer.data.end(), 0);
	break;
    case DEPTH_BUFFER:
	std::fill(depthbuffer.data.begin(), depthbuffer.data.end(), 65535);
	break;
    }
    return;
}

