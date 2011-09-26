#ifndef TEXTURE_H_GUARD
#define TEXTURE_H_GUARD
#include <string>
#include <vector>

struct Texture
{
    std::vector<unsigned int> color;
    unsigned int width;
    unsigned int height;
};


const struct Texture* ReadPNG(const std::string& name);
void BindTexture(const Texture* texture);

extern const struct Texture* currentTexture;

#endif
