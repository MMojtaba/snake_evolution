#include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <iostream>

class Program
{
public:
    Program()
    {
        id_ = glCreateProgram();
    }

    ~Program()
    {
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


private:
    unsigned int id_;
    unsigned int vertex_shader_;
    unsigned int frag_shader_;
};