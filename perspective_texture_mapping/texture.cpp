#include "texture.h"
#include <vector>
#include <png.h>

const Texture* currentTexture;


void BindTexture(const Texture* texture)
{
    currentTexture = texture;
}


const struct Texture* ReadPNG(const std::string& name)
{
    Texture *texture;
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int bit_depth, color_type, pass_num;
    png_bytep *row_pointers;
    FILE *fp;

#ifdef WIN32
    if ((fp = fopen(name.c_str(), "rb")) == NULL)
        return NULL;
#else
    if ((fp = fopen(name.c_str(), "r")) == NULL)
        return NULL;
#endif

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr){
        fclose(fp);
        return NULL;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr){
        fclose(fp);
        return NULL;
    }
    if (setjmp(png_jmpbuf(png_ptr))){
        fclose(fp);
        return NULL;
    }
    
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    
    width = info_ptr->width;
    height = info_ptr->height;
    color_type = info_ptr->color_type;
    bit_depth = info_ptr->bit_depth;

    /*
    if(color_type != PNG_COLOR_TYPE_RGBA)
	return NULL;
    */
    row_pointers = new png_bytep[height];
    for (int y=0; y<height; y++)
        row_pointers[y] = new png_byte[info_ptr->rowbytes];
    
    pass_num = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, info_ptr);   
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
    
    texture = new Texture;
    texture->width = width;
    texture->height = height;

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
	    unsigned char r,g,b,a;
	    if(color_type == PNG_COLOR_TYPE_RGBA){
		r = row_pointers[y][(x*4) + 0];
		g = row_pointers[y][(x*4) + 1];
		b = row_pointers[y][(x*4) + 2];
		a = row_pointers[y][(x*4) + 3];
            } else {
		r = row_pointers[y][(x*3) + 0];
		g = row_pointers[y][(x*3) + 1];
		b = row_pointers[y][(x*3) + 2];
		a = 127;
	    }
            texture->color.push_back((unsigned int)((a<<24) | (r<<16) | (g<<8) | b));
        }
        delete[] row_pointers[y];
    }
    delete[] row_pointers;
       
    return texture;
}


