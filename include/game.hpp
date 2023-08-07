#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "program.hpp"
#include "utils.hpp"
#include "texture.hpp"

class Game
{
public:
    Game(unsigned int win_width, unsigned int win_height): 
        play_button_selected_(true),
        in_menu_(false), //TODO change back to true
        window_width_(win_width),
        window_height_(win_height),
        score_area_height_(100),
        game_over_(false),
        width_(32.0f),
        velocity_(4.0f),
        //vertex shader code
        vs_code( 
            "#version 120\n"
            "attribute vec2 aPos;\n"
            "attribute vec2 aTexCoord;\n"
            "varying vec2 TexCoord;\n"
            "uniform vec2 uPos;\n"
            "void main() {\n"
                "float x_new = (aPos.x+uPos.x - 400.0f)/400.0f;\n" 
                "float y_new = (aPos.y+uPos.y - 300.0f)/300.0f;\n"
                "vec2 pos_new = vec2(x_new, y_new);\n"
                "gl_Position=vec4(pos_new, 1.0, 1.0);\n"
                "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
            "}"
        ),
        //fragment shader code
        fs_code(
            "#version 120\n"
            "varying vec2 TexCoord;\n"
            "uniform sampler2D texture;\n"
            "void main() {\n"
                "vec4 color = texture2D(texture, TexCoord);\n"
                "gl_FragColor = color;\n" 
            "}"
        )
    {

        //set game parameters to their default
        reset(); 

        //create shaders
        program_.create_shaders(vs_code.c_str(), fs_code.c_str());

        //add attributes: 
        glBindAttribLocation(program_.id(), 2, "aPos"); //vertex position attribute
        glBindAttribLocation(program_.id(), 3, "aTexCoord"); //texture coordinate attribute
    
        //attach shaders
        program_.attach_shaders();

        //vertex data for snake and apple
        
        float vertices[] = 
        {
            //snake head
            //positions             texture coordinates
            0.0f, 0.0f,             0.0f, 0.0f, //bottom left
            width_, 0.0f,           1.0f, 0.0f, //bottom right
            width_, width_,         1.0f, 1.0f, //top right
            0.0f, width_,           0.0f, 1.0f, //top left
            //apple
            //positions             texture coordinates
            0.0f, 0.0f,             0.0f, 0.0f, //bottom left
            width_, 0.0f,           1.0f, 0.0f, //bottom right
            width_, width_,           1.0f, 1.0f, //top right
            0.0f, width_,             0.0f, 1.0f //top left
        };

        //create buffer to store the vertices data
        // unsigned int vertex_buffer;
        // glGenBuffers(1, &vertex_buffer);
        // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set buffer as current one
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //add data to buffer

        //position of vertices attribute
        // glEnableVertexAttribArray(2); 
        // glVertexAttribPointer(2, 2, //number of position coordinates
        // GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        // 0); //where positions start

        // //texture coordinate attributes
        // glEnableVertexAttribArray(3);
        // glVertexAttribPointer(3, 2, //number of coordinates for a texture coordinate
        //     GL_FLOAT, GL_FALSE, 4*sizeof(float), //size of each vertex
        //     (void*)(2*sizeof(float)));//where texture coordinate starts
        program_.make_vertex_buffer(vertices, sizeof(vertices), 2, 3);

        //stores snake head image
        constexpr unsigned int imDims = 32;
        image_snake_head_ = new unsigned char[imDims*imDims*4];

        //load snake facing down image
        load_image("snake_down", image_snake_head_, imDims, imDims);

        if(!image_snake_head_)
        {
            std::cout << "Could not load snake head image. " << std::endl;
            exit(0);
        }
        //add image to a texture object
        Texture texture_snake_head_down(image_snake_head_, 2, imDims, imDims);
        head_down_tex_id_ = texture_snake_head_down.id();

        //load snake facing right image
        load_image("snake_right", image_snake_head_, imDims, imDims);
        if(!image_snake_head_)
        {
            std::cout << "Could not load snake head image. " << std::endl;
            exit(0);
        }
        //add image to a texture object
        Texture texture_snake_head_right(image_snake_head_, 2, imDims, imDims);
        head_right_tex_id_ = texture_snake_head_right.id();

        //load snake facing right image
        load_image("snake_left", image_snake_head_, imDims, imDims);
        if(!image_snake_head_)
        {
            std::cout << "Could not load snake head image. " << std::endl;
            exit(0);
        }
        //add image to a texture object
        Texture texture_snake_head_left(image_snake_head_, 2, imDims, imDims);
        head_left_tex_id_ = texture_snake_head_left.id();

        //load snake facing up image
        load_image("snake_head", image_snake_head_, imDims, imDims);
        if(!image_snake_head_)
        {
            std::cout << "Could not load snake head image. " << std::endl;
            exit(0);
        }
        //add image to a texture object
        Texture texture_snake_head_up(image_snake_head_, 2, imDims, imDims);
        head_up_tex_id_ = texture_snake_head_up.id();


        //load apple image
        image_apple_ = new unsigned char[imDims*imDims*4];
        load_image("apple", image_apple_, imDims, imDims);
        if(!image_apple_)
        {
            std::cout << "Could not load apple image. " << std::endl;
            exit(0);
        }
        
        //create apple texture
        Texture texture_apple(image_apple_, 3, imDims, imDims);


        program_.use();
    }

    ~Game()
    {
        // delete image_snake_head_;
        // delete image_apple_;
    }

    //process user pressing up arrow key
    void process_up()
    {
        //turn up if currently going left or right
        if(snake_dir_ == RIGHT || snake_dir_ == LEFT) 
        {
            velocity_x_ = 0;
            velocity_y_ = velocity_;
            snake_dir_ = UP;
            rotate_head(head_up_tex_id_);
        }
        
    }

    //process user pressing down arrow key
    void process_down()
    {
        //turn down if currently going left or right
        if(snake_dir_ == RIGHT || snake_dir_ == LEFT) 
        {
            velocity_x_ = 0;
            velocity_y_ = -velocity_;
            snake_dir_ = DOWN;
            rotate_head(head_down_tex_id_);
        }
    }

    //process user pressing right arrow key
    void process_right()
    {
        //turn right if currently going up or down
        if(snake_dir_ == UP || snake_dir_ == DOWN) 
        {
            velocity_x_ = velocity_;
            velocity_y_ = 0;
            snake_dir_ = RIGHT;
            rotate_head(head_right_tex_id_);
        }
    }


    //process user pressing left arrow key
    void process_left()
    {
        //turn left if currently going up or down
        if(snake_dir_ == UP || snake_dir_ == DOWN) 
        {
            velocity_x_ = -velocity_;
            velocity_y_ = 0;
            snake_dir_ = LEFT;
            rotate_head(head_left_tex_id_);
        }
    }



    //whether play button is selected
    bool play_button_selected()
    {
        return play_button_selected_;
    }

    //whether in menu
    bool in_menu()
    {
        return in_menu_;
    }

    //change play button selected to opposite
    void change_play_button_selected()
    {
        play_button_selected_ = !play_button_selected_;
    }

    //change in_menu to opposite
    void change_in_menu()
    {
        in_menu_ = !in_menu_;
    }

    //render the game
    void render_game()
    {
        program_.use();

        //render snake head
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3); 
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(program_.id(), "texture"), 2);
        glUniform2f(glGetUniformLocation(program_.id(), "uPos"), x_, y_);
        


        glDrawArrays(GL_QUADS, 0, 4);
     

        //render apple
        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(program_.id(), "texture"), 3);
        glUniform2f(glGetUniformLocation(program_.id(), "uPos"), x_apple_, y_apple_);

        glDrawArrays(GL_QUADS, 4, 4);
        glDisableVertexAttribArray(2); 
        glDisableVertexAttribArray(3); 

        // move snake
        x_ += velocity_x_;
        y_ += velocity_y_;
        // std::cout << x_ << std::endl;

        check_collision();

    }

    //resets game parameters to default
    void reset()
    {
        length_ = 1;
        score_ = 0;
        snake_dir_ = UP;
        velocity_x_ = 0.0f;
        velocity_y_ = velocity_;
        x_ = 400.0f;
        y_ = 250.0f; 
        x_apple_ = 500.0f;
        y_apple_ = 400.0f;
    }



private:

    //put a random x and y within the screen inside x and y
    void random_location(float& x, float& y)
    {
        srand(time(NULL));
        x = rand() % (window_width_-150) + 100; //between 100 and 650
        y = rand() % (window_height_-150-score_area_height_) + 100; //between 100 and 400
    }

    //checks whether the snake is colliding with something
    void check_collision()
    {
        //check walls
        if(x_ <= 0  || x_ >= window_width_-width_
            || y_ <= 0 || y_ >= window_height_ - width_ - score_area_height_)
        {
            // game_over_ = true;
            process_game_over();
            return;
        }

        //check apple collision
        if(x_ <= x_apple_ + width_ && x_ >= x_apple_ - width_ 
            && y_ <= y_apple_ + width_ && y_ >= y_apple_ - width_)
        {
            //increase score
            ++score_;
            //change apple's location
            random_location(x_apple_, y_apple_);
            // std::cout << "Touched apple, score is: " << score_ << std::endl;
        }


    }

    //handles when the player loses
    void process_game_over()
    {
        in_menu_ = true;
    }

    //updates snake head's texture for rotation effect
    void rotate_head(unsigned int tex_id)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex_id);  
    }



    bool play_button_selected_;
    bool in_menu_;
    float width_; //width of single entities (snake head, apple)
    const float velocity_;
    float velocity_x_; //velocity of snake in x direction
    float velocity_y_; //velocity of snake in y direction
    float x_; //x position of snake
    float y_; //y position of snake
    float x_apple_; //x position of apple
    float y_apple_; //y position of apple
    unsigned int length_; //length of the snake
    int score_; //user's score
    const std::string vs_code;
    const std::string fs_code;
    Program program_; //program for the game
    unsigned char* image_snake_head_;
    unsigned char* image_apple_;
    enum Direction { LEFT, RIGHT, UP, DOWN};
    Direction snake_dir_;
    const unsigned int window_width_;
    const unsigned int window_height_;
    const unsigned int score_area_height_;
    bool game_over_; //whether the user lost the game
    unsigned int head_up_tex_id_;
    unsigned int head_down_tex_id_;
    unsigned int head_right_tex_id_;
    unsigned int head_left_tex_id_;

};


#endif