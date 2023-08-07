#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Program
{
public:
    Program():
    vertex_attrib_array_size_(0)
    {
        id_ = glCreateProgram();
    }

    ~Program()
    {
        //delete shaders
        for(int i = 0 ; i < shaders_.size() ; ++i)
        {
            glDeleteShader(shaders_[i]);
        }
    }

    //adds vertex and fragment shaders
    void create_shaders(const char* vs_code, const char* fs_code)
    {
        //create shader object
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        shaders_.push_back(vs);
        shaders_.push_back(fs);


        //specify shader code
        glShaderSource(vs, 1, &vs_code, NULL);
        glShaderSource(fs, 1, &fs_code, NULL);

        //compile and check for errors
        glCompileShader(vs);
        check_compile_error(vs);
        glCompileShader(fs);
        check_compile_error(fs);

    }

    //attaches shaders to the program and links the program
    void attach_shaders()
    {
        unsigned int vs = shaders_[0];
        unsigned int fs = shaders_[1];

        glAttachShader(id_, vs);
        glAttachShader(id_, fs);
        glLinkProgram(id_);
        glValidateProgram(id_);
        check_program_error();
        // glDetachShader(id_, vs);
        // glDetachShader(id_, fs);
    }


    //Creates a buffer, stores vertex data in it, and specifies how position and texture coordinates are mapped
    void add_vertex_buffer(float* vertices, unsigned int size, 
        unsigned int posAttr_id, unsigned int texAttr_id)
    {
        unsigned int vb_id;//, posAttr_id, texAttr_id;
        glGenBuffers(1, &vb_id);
        glBindBuffer(GL_ARRAY_BUFFER, vb_id); //set buffer as current one
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); //add data to buffer
        
        //position of vertices attribute
        // posAttr_id = vertex_attrib_array_size_;
        // ++vertex_attrib_array_size_;
        glEnableVertexAttribArray(posAttr_id); 
        glVertexAttribPointer(posAttr_id, 2, //number of position coordinates
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        0); //where positions start


        //texture coordinate attributes
        // texAttr_id = vertex_attrib_array_size_;
        // ++vertex_attrib_array_size_;
        glEnableVertexAttribArray(texAttr_id);
        glVertexAttribPointer(texAttr_id, 2, //number of coordinates for a texture coordinate
            GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
            (void*)(2*sizeof(float)));//where texture coordinate starts

        //save the ids
        vertex_buffer_ids_.push_back(vb_id);
        pos_attrib_array_ids_.push_back(posAttr_id);
        tex_attrib_array_ids_.push_back(texAttr_id);
        // std::cout << "ids: " << posAttr_id << ", " << texAttr_id << std::endl;
    }

    //enables vertex array associated with given index
    void enable_vertex_array(unsigned int index, std::string type="")
    {
        if(index > vertex_buffer_ids_.size())
        {
            std::cout << "Invalid vertex array index. " << std::endl;
            return;
        }
        glEnableVertexAttribArray(pos_attrib_array_ids_[index]);
        glEnableVertexAttribArray(tex_attrib_array_ids_[index]);
        // std::cout << "get ids: " << pos_attrib_array_ids_[index] << ", " << tex_attrib_array_ids_[index] << std::endl;


    //     if(type == "position")
    //     {
    //         glEnableVertexAttribArray(pos_attrib_array_ids_[index]);
    //     }else{ //texture
    //         glEnableVertexAttribArray(tex_attrib_array_ids_[index]);
    //     }
    }

    void disable_vertex_array(unsigned int index, std::string type="")
    {
        if(index > vertex_buffer_ids_.size())
        {
            std::cout << "Invalid vertex array index. " << std::endl;
            return;
        }
        glDisableVertexAttribArray(pos_attrib_array_ids_[index]);
        glDisableVertexAttribArray(tex_attrib_array_ids_[index]);

    //     if(type == "position")
    //     {
    //         glEnableVertexAttribArray(pos_attrib_array_ids_[index]);
    //     }else{ //texture
    //         glEnableVertexAttribArray(tex_attrib_array_ids_[index]);
    //     }
    }

    //gets shader id associated with given index
    unsigned int get_shader(unsigned int index)
    {
        if(index > shaders_.size())
        {
            std::cout << "Invalid shader index. " << std::endl;
            return 0;
        }

        return shaders_[index];
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
        glUseProgram(id_);
    }

    //get vertex buffer id associated with given index
    unsigned int get_vertex_buffer(unsigned int num)
    {
        if(vertex_buffer_ids_.size() < num)
        {
            std::cout << "Invalid index, cannot get vertex buffer. " << std::endl;
            return -1; 
        }
        return vertex_buffer_ids_[num];
    }



private:
    unsigned int id_;
    // unsigned int vertex_shader_;
    // unsigned int frag_shader_;
    // unsigned int vertex_buffer_id_;
    unsigned int vertex_attrib_array_size_;
    std::vector<unsigned int> vertex_buffer_ids_;
    std::vector<unsigned int> pos_attrib_array_ids_;
    std::vector<unsigned int> tex_attrib_array_ids_;
    std::vector<unsigned int> shaders_; //contains shaders, first one is vertex, second one is fragment, follows this order

};


#endif