#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <cassert>
// #include <stdint.h>
#include "shader_code.hpp"
#include "program.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "texture.hpp"



//Function declarations. see their definition at the bottom for description of function.
static void glfwErrorCallback(int, const char*);
void keyCallback(GLFWwindow*, int, int, int, int);
GLFWwindow* init();
void draw_mm_buttons(unsigned int);
void clear_window();
unsigned int program_render_mm();


//initialize things such as glfw and glew and create a window
GLFWwindow* window = init();


//Global variables
constexpr int window_width = 800;
constexpr int window_height = 600;
Game game(window_width,window_height);


int main()
{
    
    //check for errors
    ce();

    //running program
    while(!glfwWindowShouldClose(window))
    {

        //clear the window's content
        clear_window();

        //render game
        game.render_game();
        
        //check for errors
        ce();

        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  
    //print opengl version information TODO remove
    print_gl_version();

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}








//End of main ---------------------------------------------------------------------------------

//Handle user inputs
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    game.process_input(window, key, action);
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
    // glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);
    
    //initialize glew
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Glew was not initialized." << std::endl;
        exit(0);
    }

    return window;
}


//clears window's content
void clear_window()
{
    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

