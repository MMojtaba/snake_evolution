#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <GL/glx.h>
#include <string>

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


void make_rectangle(const float left, const float bottom, const bool selected)
{
    const float width = 0.6;
    const float height = 0.25;

    glBegin(GL_QUADS);
        const float c = 1;
        glColor3f(c,c,c);
        glVertex2f(left,bottom);
        glVertex2f(left+width,bottom);
        glVertex2f(left+width,bottom+height);
        glVertex2f(left,bottom+height);
        
    glEnd();

    //draw outline too
    if(selected)
    {
        GLfloat lineWidthRange[2] = {0.0f, 0.0f};
        // glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
        // std::cout << "max line width: " << lineWidthRange[0];

        // const float offset = 0.01;
        glLineWidth(7);
        glBegin(GL_LINES);
            glColor3f(0,1,0);

            //left side's line
            glVertex2f(left, bottom);
            glVertex2f(left, bottom+height);

            //bottom side's line
            glVertex2f(left, bottom);
            glVertex2f(left+width, bottom);

            //top side's line
            glVertex2f(left, bottom+height);
            glVertex2f(left+width, bottom+height);

            //right side's line
            glVertex2f(left+width, bottom);
            glVertex2f(left+width, bottom+height);
        glEnd();
    }
}



void main_menu_button(const std::string text, const bool selected)
{
    // int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    // glLineWidth(0.3);
    
    if(text == "Play")
    {
        //draw button itself
        // const float left = -0.2;
        // const float bottom = 0;

        make_rectangle(-0.3, 0, selected);
        

    } else if(text == "Quit")
    {
        // const float left = -0.2;
        // const float bottom = -0.3;
        // const float width = 0.4;
        // const float height = 0.2;
        // glBegin(GL_QUADS);
        //     const float c = 1;
        //     glColor3f(c,c,c);
        //     glVertex2f(left,bottom);
        //     glColor3f(c,c,c);
        //     glVertex2f(left+width,bottom);
        //     glColor3f(c,c,c);
        //     glVertex2f(left+width,bottom+height);
        //     glColor3f(c,c,c);
        //     glVertex2f(left,bottom+height);
        // glEnd();
        make_rectangle(-0.3, -0.3, selected);

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


    constexpr int window_width = 800;
    constexpr int window_height = 600;
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Snake Evolution", NULL, NULL);
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


    // int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    // glViewport(0, 0, width, height);

    //Don't allow user to resize window
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false); 

    //running program
    while(!glfwWindowShouldClose(window))
    {
        // glfwSetWindowShouldClose(window, true);    
        // glClear(GL_COLOR_BUFFER_BIT);
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        // glViewport(0, 0, width, height);
        // ratio = width / (float) height;

        // glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // glOrtho(0.0f, width, height, 0.0f, 0.0f, 10.0f);
        // glMatrixMode(GL_MODELVIEW);
        // glOrtho(0, width, 0, height, -1, 1); //makes it so vertex coordinates are in pixel coordinates

        // glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        // glClear(GL_COLOR_BUFFER_BIT);
        // float ratio;
        // int width, height;
        // glfwGetFramebufferSize(window, &width, &height);
        // ratio = width / (float) height;
        // glViewport(0, 0, width, height);
        // glClear(GL_COLOR_BUFFER_BIT);
        // glMatrixMode(GL_PROJECTION);

        main_menu_button("Play", true);
        main_menu_button("Quit", false);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
  
    //destroy window
    glfwDestroyWindow(window);

    //clean up
    glfwTerminate();



    return 0;
}