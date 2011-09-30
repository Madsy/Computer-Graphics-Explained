#ifndef FRAMEBUFFER_H_GUARD
#define FRAMEBUFFER_H_GUARD
#include <SDL/SDL.h>
#include <vector>

template<typename T> struct Buffer2D
{
    Buffer2D(unsigned int width, unsigned int height) : w(width), h(height), data(width*height){}
    Buffer2D() : w(0), h(0), data(){}
    T& operator[](size_t index){ return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }

    unsigned int w;
    unsigned int h;
    std::vector<T> data;
};

extern Buffer2D<unsigned int> colorbuffer;
extern Buffer2D<unsigned short> depthbuffer;
extern SDL_Surface* screen;

enum BufferType
{
    COLOR_BUFFER=0,
    DEPTH_BUFFER
};

void InitBuffers(unsigned int width, unsigned int height);
void ClearBuffer(BufferType type);
#endif
