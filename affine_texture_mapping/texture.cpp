#include "texture.h"
#include <vector>
#include <IL/il.h>
#include <IL/ilu.h>

const Texture* currentTexture;


void BindTexture(const Texture* texture)
{
    currentTexture = texture;
}


const struct Texture* ReadPNG(const std::string& name)
{
    Texture *texture;
    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);
    if(!ilLoadImage(name.c_str())){
        return nullptr;
    }
    iluFlipImage();

    texture = new Texture;
    texture->width = ilGetInteger(IL_IMAGE_WIDTH);
    texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
    texture->color.resize(texture->width * texture->height);
    ilCopyPixels(0, 0, 0, texture->width, texture->height, 1, IL_RGBA, IL_UNSIGNED_BYTE, &texture->color[0]);
    ilDeleteImages(1, &img);

    return texture;
}


