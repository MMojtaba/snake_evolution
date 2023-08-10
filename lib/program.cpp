#include "program.hpp"

//Create program
Program::Program():
    id_(glCreateProgram()),
    vertex_shader_(glCreateShader(GL_VERTEX_SHADER)),
    frag_shader_(glCreateShader(GL_FRAGMENT_SHADER))
    {}

Program::~Program()
{
    //delete shaders
    glDeleteShader(vertex_shader_);
    glDeleteShader(frag_shader_);
}

/*
Create and compile vertex and fragment shaders.
Parameters:
    vs_code (const char*): Vertex shader code.
    fs_code (const char*): Fragment shader code. 
*/
void Program::create_shaders(const char* vs_code, const char* fs_code)
{

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
void Program::attach_shaders()
{
    glAttachShader(id_, vertex_shader_);
    glAttachShader(id_, frag_shader_);
    glLinkProgram(id_);
    glValidateProgram(id_);
    check_program_error();
    glDetachShader(id_, vertex_shader_);
    glDetachShader(id_, frag_shader_);
}


/*
Creates a buffer, stores vertex data in it, and specifies how position and texture coordinates are mapped.
Parameters:
    vertices (const float*): Float values that contain vertex data.
    size (const unsigned int): Size in bytes of vertices.
    posAttr_id (const unsigned int): Id of the vertexAttribPointer to use for position data of vertices.
    texAttr_id (const unsigned int): Id of the vertexAttribPointer to use for texture coordinate data of vertices.
*/
void Program::make_vertex_buffer(const float* vertices, const unsigned int size, 
    unsigned int const posAttr_id, unsigned int const texAttr_id)
{
    //generate buffer
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

//enable vertexAttribArrays associated with this program
void Program::enable_vertex_array()
{
    glEnableVertexAttribArray(pos_attrib_array_id_);
    glEnableVertexAttribArray(tex_attrib_array_id_);

}

//disable vertexAttribArrays associated with this program
void Program::disable_vertex_array(unsigned int index)
{
    glDisableVertexAttribArray(pos_attrib_array_id_);
    glDisableVertexAttribArray(tex_attrib_array_id_);
}



//gets id of this program
unsigned int Program::id()
{
    return id_;
}


//activates this program and its associated vertex array
void Program::use()
{
    enable_vertex_array();
    glUseProgram(id_);
}


/*
Checks if given shader had a compilation error, if so, prints error and terminates program
Parameters:
    shader (const unsigned int): Id of the shader.
*/
void Program::check_compile_error(const unsigned int shader)
{
    int compile_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
    if(compile_ok == GL_FALSE) //if error occurs
    {
        char errMessage[512];
        glGetShaderInfoLog(shader, 512, NULL, errMessage);
        std::cout << "Error compiling shader: " << std::endl;
        std::cout << errMessage << std::endl;

        //clean up
        glDeleteShader(shader);
        exit(0);
    }
}

//checks if linked program has an error, if so, prints it and terminates the program
void Program::check_program_error()
{
    int program_ok;
    glGetProgramiv(id_, GL_LINK_STATUS, &program_ok);
    char program_err_log[512];

    if(!program_ok) {
        glGetProgramInfoLog(id_, 512, NULL, program_err_log);
        std::cout << "Error linking program: " << program_err_log << std::endl;
        exit(0);
    }
    
}