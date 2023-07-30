#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <GL/glx.h>
#include <string>
#include <fstream>


void print_gl_version()
{
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL vendor : " << glGetString(GL_VENDOR  ) << std::endl;
    std::cout << "GL renderer: " << glGetString(GL_RENDERER ) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION  ) << std::endl;
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
        std::cout << "Error compiling shader: " << errMessage << "\n";

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
        std::cout << "Glew was not initialized.\n";
        return -1;
    }

    // print_gl_version();
    
    //vertex buffer
    unsigned int buffer1;
    glGenBuffers(1, &buffer1);

    glBindBuffer(GL_ARRAY_BUFFER, buffer1); //select buffer

    //put data in buffer
    float left = -0.3;
    float bottom = 0;
    float width = 0.6;
    float height = 0.3;
    float vertices[8] = {
        left, bottom,
        left+width,bottom,
        left+width,bottom+height,
        left,bottom+height
    };
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);

    //specify data layout
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);//call once per attribute
    glEnableVertexAttribArray(0);

    //vertex shader
    std::string vs_code = 
        "#version 120\n"
        "void main() {\n"
        "   gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "}"
        ;

    std::string frg_code = 
        "#version 120\n"
        "uniform vec4 u_color;\n"
        "void main() {\n"
            // "gl_FragColor = vec4(0.7, 0.99, 0.7, 1);\n"
            "gl_FragColor = u_color;\n"
        "}"
        ;

    //create program
    unsigned int program = glCreateProgram();

    create_shader(program, GL_VERTEX_SHADER, vs_code.c_str());
    create_shader(program, GL_FRAGMENT_SHADER, frg_code.c_str());
    glUseProgram(program);

    //set color
    int u_color_location = glGetUniformLocation(program, "u_color"); //get location of u_color variable used in shader code
    if(u_color_location == -1)
    {
        std::cout << "Couldn't find u_color. ";
    }
    glUniform4f(u_color_location, 0.7, 0.99, 0.7, 1.0);


    //running program
    while(!glfwWindowShouldClose(window))
    {
        //clear the window's content
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_QUADS, 0, 4); //draw buffer







        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}