#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <iostream>
// #include <vector>

class Program
{
public:
    //Create program
    Program();

    ~Program();

    /*
    Create and compile vertex and fragment shaders.
    Parameters:
        vs_code (const char*): Vertex shader code.
        fs_code (const char*): Fragment shader code. 
    */
    void create_shaders(const char* vs_code, const char* fs_code);

    //attaches shaders to the program and links the program
    void attach_shaders();

    /*
    Creates a buffer, stores vertex data in it, and specifies how position and texture coordinates are mapped.
    Parameters:
        vertices (const float*): Float values that contain vertex data.
        size (const unsigned int): Size in bytes of vertices.
        posAttr_id (const unsigned int): Id of the vertexAttribPointer to use for position data of vertices.
        texAttr_id (const unsigned int): Id of the vertexAttribPointer to use for texture coordinate data of vertices.
    */
    void make_vertex_buffer(const float* vertices, const unsigned int size, 
        unsigned int const posAttr_id, unsigned int const texAttr_id);

    //enable vertexAttribArrays associated with this program
    void enable_vertex_array();

    //disable vertexAttribArrays associated with this program
    void disable_vertex_array(unsigned int index);

    //gets id of this program
    unsigned int id();

    //activates this program and its associated vertex array
    void use();


private:
    /*
    Checks if given shader had a compilation error, if so, prints error and terminates program
    Parameters:
        shader (const unsigned int): Id of the shader.
    */
    void check_compile_error(const unsigned int shader);

    //checks if linked program has an error, if so, prints it and terminates the program
    void check_program_error();


    const unsigned int id_; //id of program
    const unsigned int vertex_shader_; //id of the vertex shader
    const unsigned int frag_shader_; //id of the fragment shader
    unsigned int pos_attrib_array_id_; //Id of the vertexAttribPointer used for positions
    unsigned int tex_attrib_array_id_;//Id of the vertexAttribPointer used for texture coordinates
};


#endif