#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
// #include <stdint.h>
#include "shader_code.hpp"
#include "program.hpp"

#define STB_IMAGE_IMPLEMENTATION //TODO remove
#include "stb_image.h"


//Function declarations. see their definition at the bottom for description of function.
void ce();
void print_gl_version();
static void glfwErrorCallback(int, const char*);
void keyCallback(GLFWwindow*, int, int, int, int);
GLFWwindow* init();
void set_texture_param();
void draw_mm_buttons(unsigned int);
void clear_window();
const char* load_image0(std::string);
unsigned char* load_image2(std::string name);
const char* load_image3(std::string name);
unsigned char* load_image(std::string name);
unsigned char* create_image();



//Global variables
bool play_button_selected = true; //whether play button is selected in the main menu


int main()
{
    //initialize things such as glfw and glew and create a window
    GLFWwindow* window = init();
    
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
    stbi_set_flip_vertically_on_load(1);

    // int size;
    // uint8_t* image_play_int = load_image(size);
    unsigned char* image_play = load_image("./images/play.bmp");
    // unsigned char* image_play = create_image();

    // unsigned char* image_play = stbi_load("./images/play.png", &imWidth, &imHeight, &imChannels, 0);
    // if(!image_play)
    // {
    //     std::cout << "could not load play image" << std::endl;
    //     return -1;
    // }
    unsigned char* image_quit = stbi_load("./images/quit.png", &imWidth, &imHeight, &imChannels, 4);
    // unsigned char* image_quit = load_image("./images/quit.bmp");
    
    // if(!image_quit)
    // {
    //     std::cout << "could not load quit image" << std::endl;
    //     return -1;
    // }

    std::cout << "size: " << imWidth << ", " << imHeight<< std::endl;
    // std::cout << image_quit;
    // for(int i = 0; i < imWidth*imHeight*4; ++i)
    // {
    //     // if(image_play[i] == '\0') break;
        
    //     image_play[i] = (unsigned char)(image_play_int[i]);
    //     // std::cout <<  +(image_quit[i]) << "," <<  +(image_quit[i+1]) << "," <<  +(image_quit[i+2]) << ","<<  +(image_quit[i+3]) << ",";
    // }
    // return 0;

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
    

    //check for errors
    ce();


    //running program
    while(!glfwWindowShouldClose(window))
    {
        //clear the window's content
        clear_window();

        //draw main menu buttons
        draw_mm_buttons(program.id());



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
    if(play_button_selected)
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
    if(!play_button_selected)
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


//loads the bmp image with the given name
const char* load_image0(std::string name)
{
     char R;
     char G;
     char B;
    std::string output;

    std::ifstream image(name, std::ios_base::binary);

    unsigned char header[14];
    unsigned char header2[40];

    image.read(reinterpret_cast<char*>(&header), sizeof(header));
    image.read(reinterpret_cast<char*>(&header2), sizeof(header2));


    // while(image.read(reinterpret_cast<char*>(&B), sizeof(B)) &&  image.read(reinterpret_cast<char*>(&G), sizeof(G)) && image.read(reinterpret_cast<char*>(&R), sizeof(R)))
    while(image.read(reinterpret_cast<char*>(&R), sizeof(R)) &&  image.read(reinterpret_cast<char*>(&G), sizeof(G)) && image.read(reinterpret_cast<char*>(&B), sizeof(B)))
    
    // while(image.read(&B, sizeof(B)), image.read(&R, sizeof(R)),image.read(&G, sizeof(G)))
    // while(image.get(B) && image.get(G) && image.get(R))
    {
        output.push_back(R);

        output.push_back(G);
        output.push_back(B);

        // image.read(reinterpret_cast<char*>(&B), sizeof(B));
        // output.push_back(B);
        // output.push_back((unsigned char)1);

    }

    const char* output_char = output.c_str();
    return output_char;
}


// #include <stdio.h>
unsigned char* load_image2(std::string name)
{
    FILE* file = fopen( name.c_str(), "rb" );

    if ( file == NULL ) return 0;
    int width = 800;
    int height = 600;
    unsigned char* data = (unsigned char *)malloc( width * height * 3);
    //int size = fseek(file,);
    fread( data, width * height * 3, 1, file );
    fclose( file );

    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;

        // int index = i*3;
        // unsigned char B,G,R;
        // B = data[index];
        // R = data[index+1];
        // G = data[index+2];

        // data[index] = B;
        // data[index+1] = G;

        // data[index+2] = R;

  }
  return data;
}

const char* load_image3(std::string img)
{
    std::string a =
    "";
    return a.c_str();
}



unsigned char* load_image(std::string name)
{
    int width = 64;
    int height = 32;
    unsigned char R;
    unsigned char G;
    unsigned char B;
    std::string output;
    unsigned char* im = new unsigned char[width*height*4];

    std::ifstream image(name, std::ios::in | std::ios_base::binary);

    unsigned char header[14];
    unsigned char header2[40];

    image.read(reinterpret_cast<char*>(&header), sizeof(header));
    image.read(reinterpret_cast<char*>(&header2), sizeof(header2));

    
    int padding = ((4- (width*3)%4)%4);

    // while(image.read(reinterpret_cast<char*>(&R), sizeof(R)) &&  image.read(reinterpret_cast<char*>(&G), sizeof(G)) && image.read(reinterpret_cast<char*>(&B), sizeof(B)))
    for(int y = 0 ; y < height ; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            unsigned char rgb[4];
            image.read(reinterpret_cast<char*>(rgb), 4);
            // image.read(reinterpret_cast<char*>(&G), sizeof(G));
            // image.read(reinterpret_cast<char*>(&B), sizeof(B));
            // output.push_back(B);

            // output.push_back(G);
            // output.push_back(R);
            im[(x+y*width)*4] = rgb[2];
            im[(x+y*width)*4+1] = rgb[1];
            im[(x+y*width)*4+2] = rgb[0];
            im[(x+y*width)*4+3] = rgb[3];
            // image.ignore(padding);
        }
    }
    // im[width*height*3-1] = '\0';
        

        

    return im;
}

unsigned char* create_image()
{
    int width = 64;
    int height = 32;
    unsigned char* im = new unsigned char[height*width*4];
    for(int y = 0 ; y < height ; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            if(x >10 && x < 20)
            {
                im[y*width + x] = 1;
                im[y*width + x + 1] = 1;
                im[y*width + x + 2] = 1;
                im[y*width + x + 3] = 1;

            }else{
                im[y*width + x] = 0;
                im[y*width + x + 1] = 0;
                im[y*width + x + 2] = 0;
                im[y*width + x + 3] = 1;

            }
            im[(y*width + x)*4] = 255;
            im[(y*width + x)*4+1] = 255;
            im[(y*width + x)*4+2] = 255;
            im[(y*width + x)*4+3] = 255;
        }
    }
    return im;
}