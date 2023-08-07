#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Program
{
public:
    Program()
    {
        id_ = glCreateProgram();
    }

    ~Program()
    {
        //delete shaders
        glDeleteShader(vertex_shader_);
        glDeleteShader(frag_shader_);

    }

    //adds vertex and fragment shaders
    void create_shaders(const char* vs_code, const char* fs_code)
    {
        //create shader object
        vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
        frag_shader_ = glCreateShader(GL_FRAGMENT_SHADER);

        //specify shader code
        glShaderSource(vertex_shader_, 1, &vs_code, NULL);
        glShaderSource(frag_shader_, 1, &fs_code, NULL);

        //compile and check for errors
        glCompileShader(vertex_shader_);
        check_compile_error(vertex_shader_);
        glCompileShader(frag_shader_);
        check_compile_error(frag_shader_);

    }

    //attaches shaders to the program and links the program
    void attach_shaders()
    {
        glAttachShader(id_, vertex_shader_);
        glAttachShader(id_, frag_shader_);
        glLinkProgram(id_);
        glValidateProgram(id_);
        check_program_error();
        glDetachShader(id_, vertex_shader_);
        glDetachShader(id_, frag_shader_);
    }


    //Creates a buffer, stores vertex data in it, and specifies how position and texture coordinates are mapped
    void make_vertex_buffer(float* vertices, unsigned int size, 
        unsigned int posAttr_id, unsigned int texAttr_id)
    {
        unsigned int vb_id;
        glGenBuffers(1, &vb_id);
        glBindBuffer(GL_ARRAY_BUFFER, vb_id); //set buffer as current one
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); //add data to buffer
        
        //position attribute
        glEnableVertexAttribArray(posAttr_id); 
        glVertexAttribPointer(posAttr_id, 2, //number of position coordinates
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        0); //where positions start


        //texture coordinate attribute
        glEnableVertexAttribArray(texAttr_id);
        glVertexAttribPointer(texAttr_id, 2, //number of coordinates for a texture coordinate
            GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
            (void*)(2*sizeof(float)));//where texture coordinate starts

        //save the ids
        pos_attrib_array_id_ = posAttr_id;
        tex_attrib_array_id_ = texAttr_id;
    }

    //enables vertex array associated with given index
    void enable_vertex_array()
    {
        glEnableVertexAttribArray(pos_attrib_array_id_);
        glEnableVertexAttribArray(tex_attrib_array_id_);

    }

    void disable_vertex_array(unsigned int index, std::string type="")
    {
        glDisableVertexAttribArray(pos_attrib_array_id_);
        glDisableVertexAttribArray(tex_attrib_array_id_);
    }



    //checks if shader had a compilation error
    void check_compile_error(unsigned int shader)
    {
        int compile_ok;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
        if(compile_ok == GL_FALSE) //if error occurs
        {
            int errLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLength);
            char* errMessage = new char[errLength];
            glGetShaderInfoLog(shader, errLength, &errLength, errMessage);
            std::cout << "Error compiling shader: " << std::endl;
            std::cout << errMessage << std::endl;

            //clean up
            glDeleteShader(shader);
            delete errMessage;
            exit(0);
        }
    }

    //checks if linked program has an error
    void check_program_error()
    {
        int program_ok;
        glGetProgramiv(id_, GL_LINK_STATUS, &program_ok);
        char* program_err_log = new char[512];
        if(!program_ok) {
            glGetProgramInfoLog(id_, 512, NULL, program_err_log);
            std::cout << "Error linking program: " << program_err_log << std::endl;
            delete program_err_log;
            exit(0);
        }
    }

    //gets id of the program
    unsigned int id()
    {
        return id_;
    }



    //use program
    void use()
    {
        enable_vertex_array();
        glUseProgram(id_);

    }




private:
    unsigned int id_;
    unsigned int vertex_shader_;
    unsigned int frag_shader_;
    unsigned int pos_attrib_array_id_;
    unsigned int tex_attrib_array_id_;
};


#endif