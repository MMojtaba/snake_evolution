#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glx.h>

//for print errr
static void glfwErrorCallback(int id, const char* err)
{
  std::cerr << err <<"\n";
}


int main()
{
    //Call error callback function to print errors when they happen
    glfwSetErrorCallback(&glfwErrorCallback);

    //create window
	if(!glfwInit()) 
    {
        std::cerr << "GLFW was not initialized. \n";
        return -1;
    }
	GLFWwindow* window = glfwCreateWindow(800, 600, "Snake Evolution", NULL, NULL);
	glfwMakeContextCurrent(window);
    if(!window)
	{
		glfwTerminate();
        std::cerr << "Window was not created. \n";
		return -1;
	}

    //running program
    while(!glfwWindowShouldClose(window))
    {

    }


    //clean up
    glfwTerminate();



    return 0;
}