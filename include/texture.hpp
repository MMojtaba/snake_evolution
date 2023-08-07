#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>


class Texture
{
public:
    Texture(unsigned char* image, unsigned int slot, unsigned int width, unsigned int height):
        slot_(slot),
        width_(width),
        height_(height)
    {
        //activate texture slot
        glActiveTexture(GL_TEXTURE0+slot_);
        //create and bind a texture object to this slot
        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);
        //set texture parameters required for rendering a texture
        set_texture_params();
        //add the image to this texture slot
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }



    unsigned int id()
    {
        return id_;
    }


private:
    void set_texture_params()
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }


    unsigned int id_; //id of texture
    unsigned int slot_; //texture slot used for this texture
    unsigned int width_; //image's width
    unsigned int height_; //image's height

};

#endif