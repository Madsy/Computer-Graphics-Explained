/*
Copyright (c) 2013, Mads Andreas Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*/

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
