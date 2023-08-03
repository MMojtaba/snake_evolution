#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION //TODO remove
#include "stb_image.h"

void ce()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Error: " << err << std::endl; 
        assert(false);
    }
}

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

static bool play_button_selected = true;

//for printing errors
static void glfwErrorCallback(int id, const char* err)
{
  std::cerr << err <<"\n";
}


//Handle user inputs
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //escape key: close game
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);   
    }

    //up and down switches the selected button in the main menu
    if((key == GLFW_KEY_UP && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS))
    {
        play_button_selected = !play_button_selected;
    }
}


int main()
{

    //set required opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    
    //Call error callback function to print errors when they happen
    glfwSetErrorCallback(&glfwErrorCallback);

    //create window
	if(!glfwInit()) 
    {
        std::cerr << "GLFW was not initialized. \n";
        return -1;
    }
    constexpr int window_width = 800;
    constexpr int window_height = 600;
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Snake Evolution", NULL, NULL);
    if(!window)
	{
        std::cerr << "Window was not created. \n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //set frame rate to refresh rate

    //Key callback for reading user inputs
    glfwSetKeyCallback(window, keyCallback);

    //Don't allow user to resize window
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false); 
    
    //initialize glew
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Glew was not initialized." << std::endl;
        return -1;
    }
    


    //shaders for play button
    std::string vs_code = 
        "#version 120\n"
        "attribute vec2 aPos;\n"
        "attribute vec2 aTexCoord;\n"
        "varying vec2 TexCoord;\n"
        "void main() {\n"
            // "gl_Position =  gl_ModelViewProjectionMatrix * vec4(aPos, 1.0);\n"
            // "gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "gl_Position = vec4(aPos, 1.0, 1.0);\n"
            "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}"
        ;
    const char* vs_code_char = vs_code.c_str();

    std::string fs_code = 
        "#version 120\n"
        "varying vec2 TexCoord;\n"
        "uniform sampler2D texture;\n"
        "uniform int uPlaySelected;\n"
        "void main() {\n"
            "vec4 color = texture2D(texture, TexCoord);\n"
            "if(uPlaySelected == 0 || TexCoord.x < 0.95 && TexCoord.x > 0.04 && TexCoord.y < 0.95 && TexCoord.y > 0.04){\n"
                "gl_FragColor = color;\n"
            "}else {\n"
                "gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n"
            "}\n"
        "}"
        ;
    const char* fs_code_char = fs_code.c_str();


    //create program
    unsigned int program = glCreateProgram();

    //create shaders
    unsigned int shader_v = glCreateShader(GL_VERTEX_SHADER);
    unsigned int shader_f = glCreateShader(GL_FRAGMENT_SHADER);
    

    //specify shader source
    glShaderSource(shader_v, 1, &vs_code_char, NULL);
    glCompileShader(shader_v);

    glShaderSource(shader_f, 1, &fs_code_char, NULL);
    glCompileShader(shader_f);

    //error handle
    int compile_ok_v;
    int compile_ok_f;
    glGetShaderiv(shader_v, GL_COMPILE_STATUS, &compile_ok_v);
    glGetShaderiv(shader_f, GL_COMPILE_STATUS, &compile_ok_f);
    if(compile_ok_v == GL_FALSE) //if error occurs
    {
        int errLength;
        glGetShaderiv(shader_v, GL_INFO_LOG_LENGTH, &errLength);
        char* errMessage = new char[errLength];
        glGetShaderInfoLog(shader_v, errLength, &errLength, errMessage);
        std::cout << "Error compiling shader: " << errMessage << std::endl;

        //clean up
        glDeleteShader(shader_v);
        glDeleteShader(shader_f);
        return 0;
    }
    if(compile_ok_f == GL_FALSE) //if error occurs
    {
        int errLength;
        glGetShaderiv(shader_f, GL_INFO_LOG_LENGTH, &errLength);
        char* errMessage = new char[errLength];
        glGetShaderInfoLog(shader_v, errLength, &errLength, errMessage);
        std::cout << "Error compiling frag shader: " << errMessage << std::endl;

        //clean up
        glDeleteShader(shader_v);
        glDeleteShader(shader_f);
        return 0;
    }

    glBindAttribLocation(program, 0, "aPos");
    // glBindAttribLocation(program, 1, "aColor");
    glBindAttribLocation(program, 1, "aTexCoord");

    //attach shader to program
    glAttachShader(program, shader_v);
    glAttachShader(program, shader_f);


    glLinkProgram(program);
    glValidateProgram(program);

    int program_ok;
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    char* program_err_log = new char[512];
    if(!program_ok) {
        glGetProgramInfoLog(program, 512, NULL, program_err_log);
        std::cout << "Error linking program: " << program_err_log << std::endl;
    }

    //cleanup
    glDeleteShader(shader_v);
    glDeleteShader(shader_f);

//position of vertices
    float left = -0.2f;
    float bottom = 0.0f;
    float width = 0.4f;
    float height = 0.2f;

    float vertices[] = {
        //positions                 texture coordinates
        left, bottom,                0.0f, 0.0f, //bottom left
        left+width,bottom,           1.0f, 0.0f, //bottom right
        left+width,bottom+height,    1.0f, 1.0f, //top right
        left,bottom+height,          0.0f, 1.0f //top left
    };

    //create buffer to store vertices
    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set buffer as current one
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //add data to buffer

    //position of vertices attribute (vertex data layout)
    glVertexAttribPointer(0, 2, //number of positions
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        0); //where positions start
    glEnableVertexAttribArray(0); 

    //texture coordinate attributes
    glVertexAttribPointer(1, 2, //number of coordinates for a texture coordinate
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        (void*)(2*sizeof(float)));//where texture coordinate starts
    glEnableVertexAttribArray(1);


    //load image
    int imWidth, imHeight, imChannels;
    stbi_set_flip_vertically_on_load(1);

    unsigned char* image = stbi_load("./images/play.png", &imWidth, &imHeight, &imChannels, 4);
    if(!image)
    {
        std::cout << "could not load image" << std::endl;
        return -1;
    }
    std::cout << "Image dimensions: " <<  imWidth << ", " << imHeight << std::endl;

    //create texture object
    unsigned int texture;
    // glActiveTexture(GL_TEXTURE0); //activate texture slot 0
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 

    //set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //set the image to the current texture slot
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    //
    // glBindAttribLocation(program, 0, "position");
    // glBindAttribLocation(program, 1, "texCoord");

    // //position of vertices attribute (vertex data layout)
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(0*sizeof(float)));//call once per attribute
    // glEnableVertexAttribArray(0);

    // //texture coordinate attributes (texture coordinates data layout)
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));//call once per attribute
    // glEnableVertexAttribArray(1);

    // create_shader(program, GL_VERTEX_SHADER, vs_code.c_str());
    // create_shader(program, GL_FRAGMENT_SHADER, frg_code.c_str());
    // glUseProgram(program);

    

    // glUniform1i(glGetUniformLocation(program, "tex"), 0);

    //set uniform color for shader
    // int u_color_location = glGetUniformLocation(program, "u_color"); //get location of u_color variable used in shader code
    // if(u_color_location == -1)
    // {
    //     // std::cout << "Couldn't find u_color. " << std::endl;
    // }
    // glUniform4f(u_color_location, 0.7, 1.0, 0.7, 1.0); //st u_color value


    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 1);
    

    
    ce();

    //running program
    while(!glfwWindowShouldClose(window))
    {
        //clear the window's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(program);

        if(play_button_selected)
        {
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 1);

        }else{
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 0);
        }

        glDrawArrays(GL_QUADS, 0, 4); //draw buffer





        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}