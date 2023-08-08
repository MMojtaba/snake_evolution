#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>
#include <cassert>
#include "image_data.hpp"


/*  load stored images if readFile is false, otherwise read from bmp file
    Parameters:
        name (string): name of the image file to get
        dest (string): destination to put the image's pixel data
        width (int): Width of the image
        height (int): Height of the image
*/
inline void load_image(const std::string name, unsigned char* dest, const int width, const int height, const bool readFile = false)
{
    std::ifstream image(name, std::ios::in | std::ios_base::binary);

    unsigned char* image_data;

    //read the header to get to data
    if(readFile)
    {
        unsigned char header[70]; //54
        image.read(reinterpret_cast<char*>(&header), sizeof(header));
    }else{
        image_data = get_image(name);
    }
    
    //read data
    for(int y = 0 ; y < height ; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            int i = (x+y*width)*4;

            if(readFile) //reads from file and ouputs to terminal so it can be saved as a variable
            {
                unsigned char rgb[4];
                image.read(reinterpret_cast<char*>(rgb), 4);
                dest[i] = rgb[2];
                dest[i+1] = rgb[1];
                dest[i+2] = rgb[0];
                dest[i+3] = rgb[3];
                std::cout << (int)dest[i] << "," << (int)dest[i+1] << "," << (int)dest[i+2] << "," <<(int)dest[i+3] << ",";
            }else
            {
                dest[i] = image_data[i];
                dest[i+1] = image_data[i+1];
                dest[i+2] = image_data[i+2];
                dest[i+3] = image_data[i+3];
            }
            
        }
    }
}



//prints opengl version information
inline void print_gl_version()
{
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL vendor : " << glGetString(GL_VENDOR  ) << std::endl;
    std::cout << "GL renderer: " << glGetString(GL_RENDERER ) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION  ) << std::endl;
    int num;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
    std::cout << "Texture Units: " << num  << std::endl;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num);
    std::cout << "Max vertex attrib: " << num  << std::endl;
}


//if an opengl error has occured when this function is called, prints the error code and terminates program
inline void ce()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Error: " << err << std::endl; 
        assert(false);
    }
}


//prints glfw errors
static void glfwErrorCallback(int id, const char* err)
{
  std::cerr << err <<"\n";
}


//clears window's content
inline void clear_window()
{
    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

#endif