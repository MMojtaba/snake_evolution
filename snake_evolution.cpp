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



//Function declarations. see their definition at the bottom for description of function.
void ce();
void print_gl_version();
static void glfwErrorCallback(int, const char*);
void keyCallback(GLFWwindow*, int, int, int, int);
GLFWwindow* init();
void set_texture_param();
void draw_mm_buttons(unsigned int);
void clear_window();
void load_image(std::string name, unsigned char*, int width, int height);
unsigned int program_render_mm();
unsigned int program_render_game();



//Global variables
Game game;


int main()
{
    //initialize things such as glfw and glew and create a window
    GLFWwindow* window = init();
    
   unsigned int program_id_mm = program_render_mm();
   unsigned int program_id_game = program_render_mm();


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
            game.render_game(program_id_game);
        }
        




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

    //Add attributes for position and textexure coordinates to be used by shaders
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
    glVertexAttribPointer(0, 2, //number of positions
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        0); //where positions start
    glEnableVertexAttribArray(0); 

    //texture coordinate attributes
    glVertexAttribPointer(1, 2, //number of coordinates for a texture coordinate
        GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        (void*)(2*sizeof(float)));//where texture coordinate starts
    glEnableVertexAttribArray(1);


    //load play and quit images
    int imWidth, imHeight, imChannels;
    imWidth = 64;
    imHeight = 32;

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


unsigned int program_render_game()
{
    ShaderCode shaderCode;
    

    Program program;


    return program.id();
}


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
        game.change_play_button_selected();
    }

    //start game if enter is pressed while selecting play in the menu
    if(game.in_menu() && key == GLFW_KEY_ENTER && game.play_button_selected())
    {
        game.change_in_menu();
    } else if(game.in_menu() && key == GLFW_KEY_ENTER && !game.play_button_selected())
    {
        //quit game if quit is selected in the menu
        glfwSetWindowShouldClose(window, true);
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


//set texture parameters
void set_texture_param()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//draws play button in main menu
void draw_play_button(unsigned int id)
{
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
    draw_play_button(id);
    draw_quit_button(id);
}

//clears window's content
void clear_window()
{
    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}



//read the speicifed bmp image
void load_image(std::string name, unsigned char* dest, int width, int height)
{
    std::ifstream image(name, std::ios::in | std::ios_base::binary);

    //read the header to get to data
    unsigned char header[70]; //54
    image.read(reinterpret_cast<char*>(&header), sizeof(header));

    //read data
    for(int y = 0 ; y < height ; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            unsigned char rgb[4];
            image.read(reinterpret_cast<char*>(rgb), 4);
            dest[(x+y*width)*4] = rgb[2];
            dest[(x+y*width)*4+1] = rgb[1];
            dest[(x+y*width)*4+2] = rgb[0];
            dest[(x+y*width)*4+3] = rgb[3];
        }
    }
}