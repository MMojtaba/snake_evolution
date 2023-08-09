#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>


class Texture
{
public:

    /*
    Intializes the texture object.
    Parameters:
        image (unsigned char*): Pointer to place in memory that contains image data that will be used for this texture.
        slot (unsigned int): The texture slot to beind this texture object to.
        width (unsigned int): The width of the image.
        height (unsigned int): The height of the image.
    */
    Texture(const unsigned char* image, unsigned int slot, 
        unsigned int width, unsigned int height);

    ~Texture() = default;


    /*
    Get id associated with this texture.
    Returns:
        id (unsigned int): Texture's id.
    */
    unsigned int id();


private:
    /*
    Set parameters required to render a texture.
    */
    void set_texture_params();

    unsigned int id_; //id of texture
    unsigned int slot_; //texture slot used for this texture
    unsigned int width_; //image's width
    unsigned int height_; //image's height

};

#endif