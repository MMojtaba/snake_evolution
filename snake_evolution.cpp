#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include "shader_code.hpp"

#define STB_IMAGE_IMPLEMENTATION //TODO remove
#include "stb_image.h"


//Function declarations. see their definition for description of function.
void ce();
void print_gl_version();
static void glfwErrorCallback(int, const char*);
void keyCallback(GLFWwindow*, int, int, int, int);
GLFWwindow* init();


//Global variables
bool play_button_selected = true;


int main()
{
    //initialize things such as glfw and glew and create a window
    GLFWwindow* window = init();
    
    //get shader code
    ShaderCode shaderCode;
    const char* vs_code_char = shaderCode.get_play_vertex();
    const char* fs_code_char = shaderCode.get_play_frag();


    //create program
    unsigned int program = glCreateProgram();

    //create shaders
    unsigned int shader_v = glCreateShader(GL_VERTEX_SHADER);
    unsigned int shader_f = glCreateShader(GL_FRAGMENT_SHADER);
    

    //specify shader source
    glShaderSource(shader_v, 1, &vs_code_char, NULL);
    glShaderSource(shader_f, 1, &fs_code_char, NULL);

    glCompileShader(shader_v);
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

    float left_quit = -0.2f;
    float bottom_quit = -0.25f;

    float vertices[] = {
        //play button
        //positions                 texture coordinates
        left, bottom,                0.0f, 0.0f, //bottom left
        left+width,bottom,           1.0f, 0.0f, //bottom right
        left+width,bottom+height,    1.0f, 1.0f, //top right
        left,bottom+height,          0.0f, 1.0f, //top left
        //quit button
        //positions                 texture coordinates
        left_quit, bottom_quit,                0.0f, 0.0f, //bottom left
        left_quit+width,bottom_quit,           1.0f, 0.0f, //bottomright
        left_quit+width,bottom_quit+height,    1.0f, 1.0f, //top right
        left_quit,bottom_quit+height,          0.0f, 1.0f //top left
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


    //load images
    int imWidth, imHeight, imChannels;
    stbi_set_flip_vertically_on_load(1);

    unsigned char* image_play = stbi_load("./images/play.png", &imWidth, &imHeight, &imChannels, 4);
    if(!image_play)
    {
        std::cout << "could not load play image" << std::endl;
        return -1;
    }
    unsigned char* image_quit = stbi_load("./images/quit.png", &imWidth, &imHeight, &imChannels, 4);
    if(!image_quit)
    {
        std::cout << "could not load quit image" << std::endl;
        return -1;
    }

    //create play texture
    unsigned int texture_play;
    glActiveTexture(GL_TEXTURE0); //activate texture slot
    glGenTextures(1, &texture_play); 
    glBindTexture(GL_TEXTURE_2D, texture_play); 

    //set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //set the image to the current texture slot
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_play);
    // glGenerateMipmap(GL_TEXTURE_2D);

    //create quit texture
    glActiveTexture(GL_TEXTURE1); //activate texture slot
    unsigned int texture_quit;
    glGenTextures(1, &texture_quit); 
    glBindTexture(GL_TEXTURE_2D, texture_quit); 

    //set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //set the image to the current texture slot
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_quit);
    // glGenerateMipmap(GL_TEXTURE_2D);



    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 1);
    
    
    ce();

    //running program
    while(!glfwWindowShouldClose(window))
    {
        //clear the window's content
        glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //activate texture and shaders
        glUseProgram(program);

        //draw play ---------------------------
        glActiveTexture(GL_TEXTURE0); //activate texture slot
        glBindTexture(GL_TEXTURE_2D, texture_play);

        //highlight play button if selected
        if(play_button_selected)
        {
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 1);

        }else{
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 0);
        }
        glUniform1i(glGetUniformLocation(program, "texture"), 0);
        glDrawArrays(GL_QUADS, 0, 4); //draw buffer


        //draw quit -----------------------------
        glActiveTexture(GL_TEXTURE1); //activate texture slot
        glBindTexture(GL_TEXTURE_2D, texture_quit);
    

        //highlight quit button if selected
        if(!play_button_selected)
        {
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 1);

        }else{
            glUniform1i(glGetUniformLocation(program, "uPlaySelected"), 0);
        }
        glUniform1i(glGetUniformLocation(program, "texture"), 1);

        glDrawArrays(GL_QUADS, 4, 4); //draw buffer


        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//End of main ---------------------------------------------------------------------------------

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

//handle glfw errors 
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

//initializes the required components (glfw, glew)
GLFWwindow* init()
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
        exit(0);
    }
    constexpr int window_width = 800;
    constexpr int window_height = 600;
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Snake Evolution", NULL, NULL);
    if(!window)
	{
        std::cerr << "Window was not created. \n";
		glfwTerminate();
		exit(0);
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
        exit(0);
    }

    return window;
}