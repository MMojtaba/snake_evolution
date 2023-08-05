#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <string>

//read the speicifed bmp image
void load_image(std::string name, unsigned char* dest, int width, int height)
{
    std::ifstream image(name, std::ios::in | std::ios_base::binary);

    //read the header to get to data
    unsigned char header[70]; //54
    image.read(reinterpret_cast<char*>(&header), sizeof(header));

    //read data
    for(int y = 0 ; y < height ; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            unsigned char rgb[4];
            image.read(reinterpret_cast<char*>(rgb), 4);
            dest[(x+y*width)*4] = rgb[2];
            dest[(x+y*width)*4+1] = rgb[1];
            dest[(x+y*width)*4+2] = rgb[0];
            dest[(x+y*width)*4+3] = rgb[3];
        }
    }
}


//set texture parameters
void set_texture_param()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//prints opengl version information
void print_gl_version()
{
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL vendor : " << glGetString(GL_VENDOR  ) << std::endl;
    std::cout << "GL renderer: " << glGetString(GL_RENDERER ) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION  ) << std::endl;
    int numTexUnits;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &numTexUnits);
    std::cout << "Texture Units: " << numTexUnits  << std::endl;
}


//if an error has occured, prints the error code and terminates program
void ce()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Error: " << err << std::endl; 
        assert(false);
    }
}


//handle glfw errors 
static void glfwErrorCallback(int id, const char* err)
{
  std::cerr << err <<"\n";
}




#endif