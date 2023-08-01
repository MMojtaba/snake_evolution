#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <GL/glx.h>
#include <string>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION //TODO remove
#include "stb_image.h"


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
    // std::cout << "Key is: " << key << "\n"; 

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

//create vertex and fragment shader based on provided shader code
void create_shader(unsigned int program, unsigned int shader_type, const char* shader_code)
{


    //create shaders
    unsigned int shader = glCreateShader(shader_type);

    //specify shader source
    glShaderSource(shader, 1, &shader_code, nullptr);

    //compile shaders
    glCompileShader(shader);

    //error handle
    int compile_ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
    if(compile_ok == GL_FALSE) //if error occurs
    {
        int errLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLength);
        char* errMessage = new char[errLength];
        glGetShaderInfoLog(shader, errLength, &errLength, errMessage);
        std::cout << "Error compiling shader: " << errMessage << std::endl;

        //clean up
        glDeleteShader(shader);
        return;
    }

    //attach shader to program
    glAttachShader(program, shader);
    glLinkProgram(program);
    glValidateProgram(program);

    //cleanup
    glDeleteShader(shader);
    glDetachShader(program, shader);

}







int main()
{

    // unsigned char* image = read_bmp("images/play_asperite.bmp");

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
		glfwTerminate();
        std::cerr << "Window was not created. \n";
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
    // print_gl_version();
    
    //vertex array object
    // unsigned int vertex_array;
    // glGenVertexArrays(1, &vertex_array);
    // glBindVertexArray(vertex_array);

    

    //put data in buffer
    float left = -0.3f;
    float bottom = 0.0f;
    float width = 0.6f;
    float height = 0.3f;
    float vertices[8+8] = {
        left, bottom, 0.0f, 0.0f,
        left+width,bottom, 1.0f, 0.0f, 
        left+width,bottom+height, 1.0f, 1.0f, 
        left,bottom+height, 0.0f, 1.0f
    };
    // unsigned int indices[] = {0, 1, 2, 3};
    //vertex buffer
    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //select buffer
    glBufferData(GL_ARRAY_BUFFER, 4*2 * sizeof(float), vertices, GL_STATIC_DRAW);


    //position of vertices attribute (vertex data layout)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(0*sizeof(float)));//call once per attribute
    glEnableVertexAttribArray(0);

    //texture coordinate attributes (texture coordinates data layout)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));//call once per attribute
    glEnableVertexAttribArray(1);
    

    //add play button texture-------------------------------------------
    // glGenTextures(1, );

    int imWidth, imHeight, imChannels;
    // stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load("./images/play_asperite.png", &imWidth, &imHeight, &imChannels, 4);
    if(!image)
    {
        std::cout << "could not load image" << std::endl;
        return -1;
    }
    std::cout << "Image dimensions: " <<  imWidth << ", " << imHeight << std::endl;

    unsigned int texture;
    // glEnable(GL_TEXTURE_2D);
    // glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //set texture parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // glUniform1i(glGetUniformLocation(program, "tex"), 0);

    stbi_image_free(image);

    //vertex shader
    // std::string vs_code = 
    //     "#version 120\n"
    //     // "attribute vec2 tex;\n"
    //     "varying vec2 texCoord;\n"
    //     // "varying vec2 out_tex;\n"
    //     "void main() {\n"
    //         // "gl_TexCoord[0] = gl_MultiTexCoord0;\n"
    //         "gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    //         // "gl_Position = ftransform();\n"
    //         "texCoord = gl_MultiTexCoord0.xy;\n"
    //     "}"
    //     ;
    std::string vs_code = 
        "#version 120\n"
        "void main() {\n"
            "gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "}"
        ;


    // std::string frg_code = 
    //     "#version 120\n"
    //     "uniform vec4 u_color;\n"
    //     "uniform sampler2D tex;\n"
    //     "varying vec2 texCoord;\n"
    //     "void main() {\n"
    //         // "gl_FragColor = vec4(0.7, 0.99, 0.7, 1);\n"
    //         "gl_FragColor = u_color;\n"
    //         // "gl_FragColor = texture2D(tex, texCoord);\n"
    //     "}"
    //     ;
    std::string frg_code = 
        "#version 120\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
            "gl_FragColor = texture2D(tex, gl_TexCoord[0].st);\n"
        "}"
        ;

    //create program
    unsigned int program = glCreateProgram();

    create_shader(program, GL_VERTEX_SHADER, vs_code.c_str());
    create_shader(program, GL_FRAGMENT_SHADER, frg_code.c_str());
    glUseProgram(program);

    //
    glUniform1i(glGetUniformLocation(program, "tex"), 0);



    //set uniform color for shader
    int u_color_location = glGetUniformLocation(program, "u_color"); //get location of u_color variable used in shader code
    if(u_color_location == -1)
    {
        // std::cout << "Couldn't find u_color. " << std::endl;
    }
    glUniform4f(u_color_location, 0.7, 1.0, 0.7, 1.0); //st u_color value


    

    

    //running program
    while(!glfwWindowShouldClose(window))
    {
        //clear the window's content

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // glBindVertexArray(vertex_array);
        glUseProgram(program);
        // glBindVertexArray(vertex_array);
        

        glDrawArrays(GL_QUADS, 2, 4); //draw buffer
        // glDrawElements(GL_QUADS, 8, GL_UNSIGNED_INT, 0);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);






        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}