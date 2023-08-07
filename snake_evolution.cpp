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
    
    
    unsigned int program_id_mm = program_render_mm();

    //check for errors
    ce();

    //running program
    while(!glfwWindowShouldClose(window))
    {

        //clear the window's content
        clear_window();

        if(game.in_menu()){
            //draw main menu
            draw_mm_buttons(program_id_mm);
        }else{
            //draw game
            game.render_game();
        }
        

        ce();

        glfwSwapBuffers(window);//swap buffer
        glfwPollEvents();//poll for events (such as quit)
    }
  

    //clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}








//End of main ---------------------------------------------------------------------------------


//do necessary things to render the main menu and return the corresponding program id
unsigned int program_render_mm()
{

 //get shader code
    ShaderCode shaderCode;

    const char* vs_code = shaderCode.get_play_vertex();
    const char* fs_code = shaderCode.get_play_frag();

    //create program and create shaders
    Program program;
    program.create_shaders(vs_code, fs_code);

    //Add attributes for position and texture coordinates to be used by shaders
    glBindAttribLocation(program.id(), 0, "aPos");
    glBindAttribLocation(program.id(), 1, "aTexCoord");

    //attach shader to program
    program.attach_shaders();




    //vertex data for main menu
    float left = -0.2f;
    float bottom = 0.0f;
    float width = 0.4f;
    float height = 0.2f;

    float left_quit = -0.2f;
    float bottom_quit = -0.25f;

    float vertices_mm[] = {
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


    //create buffer to store vertices for main menu
    unsigned int vertex_buffer_mm;
    glGenBuffers(1, &vertex_buffer_mm);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_mm); //set buffer as current one
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_mm), vertices_mm, GL_STATIC_DRAW); //add data to buffer

    //position of vertices attribute (vertex data layout)
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 2, //number of positions
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        0); //where positions start

    //texture coordinate attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, //number of coordinates for a texture coordinate
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        (void*)(2*sizeof(float)));//where texture coordinate starts


    //load play and quit images
    int imWidth, imHeight, imChannels;
    imWidth = 64;
    imHeight = 32;

    // unsigned char* image_play = new unsigned char[imWidth*imHeight*4];
    unsigned char* image_play = new unsigned char[imWidth*imHeight*4];

    load_image("./images/play.bmp", image_play, imWidth, imHeight);
    if(!image_play)
    {
        std::cout << "could not load play image" << std::endl;
        return -1;
    }

    unsigned char* image_quit = new unsigned char[imWidth*imHeight*4];
    load_image("./images/quit.bmp", image_quit, imWidth, imHeight);
    

    //create play texture
    unsigned int texture_play;
    glActiveTexture(GL_TEXTURE0); //activate texture slot
    glGenTextures(1, &texture_play); 
    glBindTexture(GL_TEXTURE_2D, texture_play); 
    //set texture parameters
    set_texture_param();
    //set the image to the current texture slot
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_play);

    //create quit texture
    unsigned int texture_quit;
    glActiveTexture(GL_TEXTURE1); //activate texture slot
    glGenTextures(1, &texture_quit); 
    glBindTexture(GL_TEXTURE_2D, texture_quit); 
    set_texture_param();//set texture parameters
    //set the image to the current texture slot
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_quit);

    //use program
    program.use();

    //set shader variable for selecting main menu buttons
    glUniform1i(glGetUniformLocation(program.id(), "uPlaySelected"), 1);

    return program.id();
}






//Handle user inputs
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //for debugging: TODO remove
    if(key==GLFW_KEY_BACKSLASH && action == GLFW_PRESS)
    {
        game.change_in_menu();
    }

    //escape key: close game
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);   
    }

    //up and down switches the selected button in the main menu
    if((key == GLFW_KEY_UP && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS))
    {
        game.change_play_button_selected();
    }

    //start game if enter is pressed while selecting play in the menu
    if(game.in_menu() && key == GLFW_KEY_ENTER && game.play_button_selected())
    {
        game.reset();
        game.change_in_menu();
    } else if(game.in_menu() && key == GLFW_KEY_ENTER && !game.play_button_selected())
    {
        //quit game if quit is selected in the menu
        glfwSetWindowShouldClose(window, true);
    }

    //process directions when in game
    if(!game.in_menu())
    {
        if(key == GLFW_KEY_UP && action == GLFW_PRESS)
            game.process_up();
        else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
            game.process_down();
        else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
            game.process_right();
        else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
            game.process_left();
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




//draws play button in main menu
void draw_play_button(unsigned int id)
{
    glUseProgram(id);
    // glEnableVertexAttribArray(0);

    //highlight play button if selected
    if(game.play_button_selected())
    {
        glUniform1i(glGetUniformLocation(id, "uPlaySelected"), 1);

    }else{
        glUniform1i(glGetUniformLocation(id, "uPlaySelected"), 0);
    }
    glActiveTexture(GL_TEXTURE0); //activate texture slot 0 for rendering play button
    glUniform1i(glGetUniformLocation(id, "texture"), 0); //update texture variable to play's image
    glDrawArrays(GL_QUADS, 0, 4); //draw play button
}

//darws quit button in main menu
void draw_quit_button(unsigned int id)
{
    glUseProgram(id);
    // glEnableVertexAttribArray(1);
    //highlight quit button if selected
    if(!game.play_button_selected())
    {
        glUniform1i(glGetUniformLocation(id, "uPlaySelected"), 1);

    }else{
        glUniform1i(glGetUniformLocation(id, "uPlaySelected"), 0);
    }
    glActiveTexture(GL_TEXTURE1); //activate texture slot 1 for rendering quit button
    glUniform1i(glGetUniformLocation(id, "texture"), 1);//update texture variable to quit's image
    glDrawArrays(GL_QUADS, 4, 4); //draw buffer
}

//draw play and quit buttons in the main menu
void draw_mm_buttons(unsigned int id)
{
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    draw_play_button(id);
    draw_quit_button(id);
    // glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);

}

//clears window's content
void clear_window()
{
    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

