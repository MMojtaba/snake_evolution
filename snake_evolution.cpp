#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <GL/glx.h>

//for print errr
static void glfwErrorCallback(int id, const char* err)
{
  std::cerr << err <<"\n";
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "Key is: " << key << "\n";
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);   
    }
}




int main()
{


    //set required opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //Call error callback function to print errors when they happen
    glfwSetErrorCallback(&glfwErrorCallback);

    //create window
	if(!glfwInit()) 
    {
        std::cerr << "GLFW was not initialized. \n";
        return -1;
    }



	GLFWwindow* window = glfwCreateWindow(640, 480, "Snake Evolution", NULL, NULL);
	glfwMakeContextCurrent(window);
    if(!window)
	{
		glfwTerminate();
        std::cerr << "Window was not created. \n";
		return -1;
	}

    //Key callback for reading user inputs
    glfwSetKeyCallback(window, keyCallback);
    
    // std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GL vendor : " << glGetString(GL_VENDOR  ) << std::endl;
    // std::cout << "GL renderer: " << glGetString(GL_RENDERER ) << std::endl;
    // std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION  ) << std::endl;


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);




    //running program
    while(!glfwWindowShouldClose(window))
    {
        // glfwSetWindowShouldClose(window, true);    
        // glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

    }
  
    //destroy window
    glfwDestroyWindow(window);

    //clean up
    glfwTerminate();



    return 0;
}