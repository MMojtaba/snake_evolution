#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
// #include <vector>
#include <deque>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "program.hpp"
#include "utils.hpp"
#include "texture.hpp"

class Game
{
public:
    /*
    Initializes the game object to the given width and height.
    Parameters:
        win_width (const unsigned int): The width of game's window.
        win_height (const unsigned int): The height of the game's window.
    */
    Game(const unsigned int win_width, const unsigned int win_height): 
        play_button_selected_(true),
        in_menu_(true), //TODO change back to true
        window_width_(win_width),
        window_height_(win_height),
        score_area_height_(100),
        entity_width_(32.0f),
        velocity_(4.0f),
        //vertex shader code for game and main menu
        vs_code_game_( 
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
        //fragment shader code for game
        fs_code_game_(
            "#version 120\n"
            "varying vec2 TexCoord;\n"
            "uniform sampler2D texture;\n"
            "void main() {\n"
                "vec4 color = texture2D(texture, TexCoord);\n"
                "gl_FragColor = color;\n" 
            "}"
        ),
        //fragment shader code for the menu
        fs_code_mm_(
            "#version 120\n"
            "varying vec2 TexCoord;\n"
            "uniform sampler2D texture;\n"
            "uniform int uPlaySelected;\n"
            "void main() {\n"
                "vec4 color = texture2D(texture, TexCoord);\n"
                "if(uPlaySelected == 0 || TexCoord.x < 0.95 && TexCoord.x > 0.04 && TexCoord.y < 0.95 && TexCoord.y > 0.04){\n"
                    "gl_FragColor = color;\n"
                "}else {\n" //draw border
                    "gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n" 
                "}\n"
            "}"
        )
    {

        //set game parameters to their default
        reset_game(); 

        //create shaders
        program_menu_.create_shaders(vs_code_game_.c_str(), fs_code_mm_.c_str());
        program_game_.create_shaders(vs_code_game_.c_str(), fs_code_game_.c_str());


        //add attributes: 

        //main menu attributes
        glBindAttribLocation(program_menu_.id(), 0, "aPos");//vertex position attribute
        glBindAttribLocation(program_menu_.id(), 1, "aTexCoord");//texture coordinate attribute

        //game attributes
        glBindAttribLocation(program_game_.id(), 2, "aPos"); 
        glBindAttribLocation(program_game_.id(), 3, "aTexCoord"); 
    
        //attach shaders
        program_game_.attach_shaders(); //main menu
        program_menu_.attach_shaders(); //game

        //vertex data for snake and apple
        const float top_area_begin = (float)window_height_-(float)score_area_height_;
        const float vertices_game[] = 
        {
            //snake head
            //positions             texture coordinates
            0.0f, 0.0f,             0.0f, 0.0f, //bottom left
            entity_width_, 0.0f,           1.0f, 0.0f, //bottom right
            entity_width_, entity_width_,         1.0f, 1.0f, //top right
            0.0f, entity_width_,           0.0f, 1.0f, //top left
            //apple
            //positions                     texture coordinates
            0.0f, 0.0f,                     0.0f, 0.0f, //bottom left
            entity_width_, 0.0f,            1.0f, 0.0f, //bottom right
            entity_width_, entity_width_,   1.0f, 1.0f, //top right
            0.0f, entity_width_,            0.0f, 1.0f, //top left
            //line seperating score and game area
            0.0f,  top_area_begin,                    0.0f, 0.0f,
            (float)window_width_, top_area_begin,      0.0f, 0.1f,
            //score text
            16.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            16.0f+entity_width_*3.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            16.0f+entity_width_*3.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            16.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
            //first score digit
            16.0f+entity_width_*4.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            16.0f+entity_width_*5.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            16.0f+entity_width_*5.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            16.0f+entity_width_*4.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
            //second score digit
            entity_width_*6.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            entity_width_*7.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            entity_width_*7.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            entity_width_*6.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
        };

        //vertex data for main menu
        constexpr float width = 170.0f;
        constexpr float height = 60.0f;
        const float left = (window_width_ - width)/2.0f;
        const float bottom = window_height_/2.0f ;
        const float bottom_quit = bottom - height - 20.0f;

        const float vertices_mm[] = {
            //play button
            //positions                 texture coordinates
            left, bottom,                0.0f, 0.0f, //bottom left
            left+width,bottom,           1.0f, 0.0f, //bottom right
            left+width,bottom+height,    1.0f, 1.0f, //top right
            left,bottom+height,          0.0f, 1.0f, //top left
            //quit button
            //positions                 texture coordinates
            left, bottom_quit,                0.0f, 0.0f, //bottom left
            left+width,bottom_quit,           1.0f, 0.0f, //bottomright
            left+width,bottom_quit+height,    1.0f, 1.0f, //top right
            left,bottom_quit+height,          0.0f, 1.0f, //top left
            //line seperating score and game area
            0.0f,  top_area_begin,                    0.0f, 0.0f,
            (float)window_width_, top_area_begin,      0.0f, 0.1f,
            //score text
            16.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            16.0f+entity_width_*3.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            16.0f+entity_width_*3.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            16.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
            //first score digit
            16.0f+entity_width_*4.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            16.0f+entity_width_*5.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            16.0f+entity_width_*5.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            16.0f+entity_width_*4.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
            //second score digit
            entity_width_*6.0f, top_area_begin+entity_width_,      0.0f, 0.0f, //bottom left
            entity_width_*7.0f, top_area_begin+entity_width_, 1.0f, 0.0f,//bottom right
            entity_width_*7.0f, top_area_begin+entity_width_*2.0f, 1.0f, 1.0f, //top right
            entity_width_*6.0f, top_area_begin+entity_width_*2.0f, 0.0f, 1.0f, //top left
        };

        //create buffer to store vertices (position and texture)
        program_menu_.make_vertex_buffer(vertices_mm, sizeof(vertices_mm), 0, 1);
        program_game_.make_vertex_buffer(vertices_game, sizeof(vertices_game), 2, 3);

        //loads all images needed for the game and stores them in textures
        load_all_textures();


        //use program
        program_menu_.use();
        program_game_.use();

    }

    ~Game()
    {
        // delete image_snake_head_;
        // delete image_apple_;
    }

    

    //render the game
    void render_game()
    {
        //keeps track of where in the vertex array the needed data is
        unsigned int render_array_offset = 0;

        if(in_menu_) //render main menu
        {

            //use main menu program
            program_menu_.use();
            
            //render play button
            if(play_button_selected())
            {
                glUniform1i(glGetUniformLocation(program_menu_.id(), "uPlaySelected"), 1);

            }else{
                glUniform1i(glGetUniformLocation(program_menu_.id(), "uPlaySelected"), 0);
            }
            glActiveTexture(GL_TEXTURE0); //activate texture slot 0 for rendering play button
            glUniform1i(glGetUniformLocation(program_menu_.id(), "texture"), 0); //update texture variable to play's image
            glDrawArrays(GL_QUADS, 0, 4); //draw play button
            render_array_offset += 4;
            
            //render quit button
            if(!play_button_selected())
            {
                glUniform1i(glGetUniformLocation(program_menu_.id(), "uPlaySelected"), 1);

            }else{
                glUniform1i(glGetUniformLocation(program_menu_.id(), "uPlaySelected"), 0);
            }
            glActiveTexture(GL_TEXTURE1); //activate texture slot 1 for rendering quit button
            glUniform1i(glGetUniformLocation(program_menu_.id(), "texture"), 1);//update texture variable to quit's image
            glDrawArrays(GL_QUADS, 4, 4); //draw buffer
            render_array_offset += 4;

            glUniform1i(glGetUniformLocation(program_menu_.id(), "uPlaySelected"), 0);
            
            //render score if its not 0
            if(score_ != 0)
                render_score(render_array_offset);

        }else //render game
        {
            //use game program
            program_game_.use();

            //render snake head
            activate_texture(2, x_, y_);
            glDrawArrays(GL_QUADS, 0, 4);
            render_array_offset += 4;
        
            //render apple
            activate_texture(3, x_apple_, y_apple_);
            glDrawArrays(GL_QUADS, 4, 4);
            render_array_offset += 4;

            //render score
            render_score(render_array_offset);

            //propegate the prev position of the snake through the body after a period of time
            if(float(clock() - timer_)/CLOCKS_PER_SEC > 0.005f)
            {
                
                //increase queue's length if snake should be larger
                while(length_ > snake_body_locX_.size())
                {
                    snake_body_locX_.push_back(-100.0f);
                    snake_body_locY_.push_back(-100.0f);

                }

                //discard the position at the end of the queue
                if(snake_body_locX_.size() > 0)
                {
                    snake_body_locX_.pop_back();
                    snake_body_locY_.pop_back();
                    // popped_ = true;
                }

                //propegate values towards the tail of the snake
                for(int i = 0; i < snake_body_locX_.size() ; ++i)
                {
                    snake_body_locX_.push_front(snake_body_locX_.back());
                    snake_body_locY_.push_front(snake_body_locY_.back());

                    snake_body_locX_.pop_back();
                    snake_body_locY_.pop_back();

                }

                //add the new position value to the queue
                if(length_ > snake_body_locX_.size()) //popped_
                {
                    snake_body_locX_.push_front(x_prev_);
                    snake_body_locY_.push_front(y_prev_);
                    // popped_ = false;
                }


                //set value of prev position to current one
                x_prev_ = x_;
                y_prev_ = y_;
                
                //reset timer
                timer_ = clock();
            }


            //render snake body
            for(int i = 0; i < snake_body_locX_.size(); ++i)
            {
                activate_texture(7, snake_body_locX_[i], snake_body_locY_[i]);
                glDrawArrays(GL_QUADS, 0, 4);
            }
            


            // move snake
            x_ += velocity_x_;
            y_ += velocity_y_;

            //check for collision
            process_collision(); //TODO move up?
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

    //resets game parameters to default
    void reset_game()
    {
        length_ = 0;
        score_ = 0;
        snake_dir_ = UP;
        velocity_x_ = 0.0f;
        velocity_y_ = velocity_;
        x_ = 400.0f;
        y_ = 250.0f; 
        x_apple_ = 500.0f;
        y_apple_ = 400.0f;
        rotate_head(head_up_tex_id_);//reset snake texture
        snake_body_locX_.clear();
        snake_body_locY_.clear();

        timer_ = clock();
        x_prev_ = x_;
        y_prev_ = y_;
        // popped_ = false; TODO remove

    }

    //handle user input
    void process_input(GLFWwindow* window, const int key, const int action)
    {
        //for debugging: TODO remove
        if(key==GLFW_KEY_BACKSLASH && action == GLFW_PRESS)
        {
            change_in_menu();
        }

        //escape key: close game
        if(key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, true);   
        }

        //up and down switches the selected button in the main menu
        if((key == GLFW_KEY_UP && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS))
        {
            change_play_button_selected();
        }

        //start game if enter is pressed while selecting play in the menu
        if(in_menu() && key == GLFW_KEY_ENTER && play_button_selected())
        {
            reset_game();
            change_in_menu();
        } else if(in_menu() && key == GLFW_KEY_ENTER && !play_button_selected())
        {
            //quit game if quit is selected in the menu
            glfwSetWindowShouldClose(window, true);
        }

        //process directions when in game
        if(!in_menu())
        {
            if(key == GLFW_KEY_UP && action == GLFW_PRESS)
                process_up();
            else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
                process_down();
            else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
                process_right();
            else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
                process_left();
        }
    }

    

private:

    //put a random x and y value within the screen inside x and y
    void random_location(float& x, float& y)
    {
        srand(time(NULL));
        x = rand() % (window_width_-150) + 100; //between 100 and 650
        y = rand() % (window_height_-150-score_area_height_) + 100; //between 100 and 400
    }

    //checks whether the snake is colliding with something and processes it
    void process_collision()
    {
        //check walls
        if(x_ <= 0  || x_ >= window_width_-entity_width_
            || y_ <= 0 || y_ >= window_height_ - entity_width_ - score_area_height_)
        {
            process_game_over();
            return;
        }

        //check apple collision
        if(x_ <= x_apple_ + entity_width_ && x_ >= x_apple_ - entity_width_ 
            && y_ <= y_apple_ + entity_width_ && y_ >= y_apple_ - entity_width_)
        {
            //increase score
            ++score_;
            ++length_;
            //change apple's location
            random_location(x_apple_, y_apple_);
        }

        //check collision with snake's body
        for(int i = 0; i < snake_body_locX_.size() ; ++i)
        {
            float body_x = snake_body_locX_[i];
            float body_y = snake_body_locY_[i];
            
            if(x_+10.0f >= body_x && x_+entity_width_-10.0f <= body_x+entity_width_
                && y_+10.0f >= body_y && y_+entity_width_-10.0f <= body_y+entity_width_)
            {
                // std::cout << "collision " << std::endl;
                process_game_over();
                return;
            }
        }


    }

    //handles when the player loses
    void process_game_over()
    {
        in_menu_ = true;
    }

    /*
    Updates snake head's texture for rotation effect
    Parameters:
        tex_id (const unsigned int): The id of the texture to use.
    */
    void rotate_head(const unsigned int tex_id)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex_id);  
    }

    //loads all the textures needed for the game and menu
    void load_all_textures()
    {
        //stores snake head image
        constexpr unsigned int imDims = 32;
        image_snake_head_ = new unsigned char[imDims*imDims*4];

        //load snake facing down image and create a texture from it
        load_image("snake_down", image_snake_head_, imDims, imDims);
        //add image to a texture object
        Texture texture_snake_head_down(image_snake_head_, 2, imDims, imDims);
        head_down_tex_id_ = texture_snake_head_down.id();

        //load snake facing right image and create a texture from it
        load_image("snake_right", image_snake_head_, imDims, imDims);
        Texture texture_snake_head_right(image_snake_head_, 2, imDims, imDims);
        head_right_tex_id_ = texture_snake_head_right.id();

        //load snake facing right image and create a texture from it
        load_image("snake_left", image_snake_head_, imDims, imDims);
        //add image to a texture object
        Texture texture_snake_head_left(image_snake_head_, 2, imDims, imDims);
        head_left_tex_id_ = texture_snake_head_left.id();

        //load snake facing right image and create a texture from it
        load_image("snake_body", image_snake_head_, imDims, imDims);
        //add image to a texture object
        Texture texture_snake_body(image_snake_head_, 7, imDims, imDims);

        //load snake facing up image and create a texture from it
        load_image("snake_head", image_snake_head_, imDims, imDims);
        //add image to a texture object
        Texture texture_snake_head_up(image_snake_head_, 2, imDims, imDims);
        head_up_tex_id_ = texture_snake_head_up.id();


        //load apple image and create a texture from it
        image_apple_ = new unsigned char[imDims*imDims*4];
        load_image("apple", image_apple_, imDims, imDims);
        //create apple texture
        Texture texture_apple(image_apple_, 3, imDims, imDims);


        //load play button image and create a texture from it
        constexpr unsigned int imWidth = 64;
        constexpr unsigned int imHeight = 32;
        unsigned char* image = new unsigned char[imWidth*imHeight*4];
        load_image("play", image, imWidth, imHeight);
        Texture texture_play(image, 0, imWidth, imHeight);

        //load quit button image  and create a texture from it
        load_image("quit", image, imWidth, imHeight);
        Texture texture_quit(image, 1, imWidth, imHeight);
        
        //load score image and create a texture from it
        load_image("score", image, imWidth, imHeight);
        Texture texture_score(image, 4, imWidth, imHeight);

        //load digits (0 to 9) images and and create textures from them
        constexpr unsigned int imDigit_width = 4;
        constexpr unsigned int imDigit_height = 8;
        unsigned char* image_digit = new unsigned char[imDigit_width*imDigit_height*4];
        //zero
        load_image("zero", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit0(image_digit, 5, imDigit_width, imDigit_height);
        digits_[0] = texture_digit0.id();
        //one
        load_image("one", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit1(image_digit, 6, imDigit_width, imDigit_height);
        digits_[1] = texture_digit1.id();
        //two
        load_image("two", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit2(image_digit, 6, imDigit_width, imDigit_height);
        digits_[2] = texture_digit2.id();
        //three
        load_image("three", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit3(image_digit, 6, imDigit_width, imDigit_height);
        digits_[3] = texture_digit3.id();
        //four
        load_image("four", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit4(image_digit, 6, imDigit_width, imDigit_height);
        digits_[4] = texture_digit4.id();
        //five
        load_image("five", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit5(image_digit, 6, imDigit_width, imDigit_height);
        digits_[5] = texture_digit5.id();
        //six
        load_image("six", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit6(image_digit, 6, imDigit_width, imDigit_height);
        digits_[6] = texture_digit6.id();
        //seven
        load_image("seven", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit7(image_digit, 6, imDigit_width, imDigit_height);
        digits_[7] = texture_digit7.id();
        //eight
        load_image("eight", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit8(image_digit, 6, imDigit_width, imDigit_height);
        digits_[8] = texture_digit8.id();
        //nine
        load_image("nine", image_digit, imDigit_width, imDigit_height);
        Texture texture_digit9(image_digit, 6, imDigit_width, imDigit_height);
        digits_[9] = texture_digit9.id();
        
    }


    /*
    Render the score area and score.
    Parameters:
        arrayOffset (unsigned int&): Where the data related to score starts in the vertex buffer
    */
    void render_score(unsigned int& arrayOffset)
    {
        
        //render line seperating score and game area
        activate_texture(3, 0.0f, 0.0f);
        glLineWidth(6);
        glDrawArrays(GL_LINES, arrayOffset, 2);
        arrayOffset += 2;

        //render score text
        activate_texture(4, 0.0f, 0.0f);
        glDrawArrays(GL_QUADS, arrayOffset, 4);
        arrayOffset += 4;


        //get the first and second digit as a number
        unsigned int digit1 = score_ / 10;
        unsigned int digit2 = score_ % 10;

        //bind the first digit to its texture slot and activate it
        activate_texture(5, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, digits_[digit1]);  
        glDrawArrays(GL_QUADS, arrayOffset, 4);
        arrayOffset += 4;

        //bind the first second to its texture slot and activate it
        activate_texture(6, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, digits_[digit2]);  
        glDrawArrays(GL_QUADS, arrayOffset, 4);
        arrayOffset += 4;

    }

    /*
    Activates the texture stored at given slot, and sets its position attribute to provided values.
    Parameters:
        slot (const unsigned int): The texture to use.
        posX (const float): The x position to set as the texture attribute.
        posY (const float): The y position to set as the texture attribute.
    */
    void activate_texture(const unsigned int slot, const float posX, const float posY)
    {
        unsigned int prog_id;
        if(in_menu_)
        {
            prog_id = program_menu_.id();
        }else{
            prog_id = program_game_.id();
        }
        glActiveTexture(GL_TEXTURE0+slot);
        glUniform1i(glGetUniformLocation(prog_id, "texture"), slot);
        glUniform2f(glGetUniformLocation(prog_id, "uPos"), posX, posY);
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


    const float entity_width_; //width of single gameplay entities (snake head, apple)
    const unsigned int window_width_; //game window's width
    const unsigned int window_height_; //game window's height
    const unsigned int score_area_height_; //score area height


    bool play_button_selected_; //whether the play button is selected in the mneu
    bool in_menu_; //whether the use is in the menu
    int score_; //user's score
    clock_t timer_; //used as a timer to update snake's body location

    const float velocity_; //how fast the snake moves
    float velocity_x_; //velocity of snake in x direction
    float velocity_y_; //velocity of snake in y direction
    float x_; //x position of snake's head
    float y_; //y position of snake's head
    float x_prev_; //previous x position of snake's head
    float y_prev_; //previous y position of snake's head
    unsigned int length_; //length of the snake
    std::deque<float> snake_body_locX_; //contains the x position of the snake body
    std::deque<float> snake_body_locY_; //contains the y position of the snake body
    float x_apple_; //x position of apple
    float y_apple_; //y position of apple

    //shader codes
    const std::string vs_code_game_; 
    const std::string fs_code_game_;
    const std::string fs_code_mm_;

    //game and menu program
    Program program_game_; 
    Program program_menu_; 
    
    //snake's direction
    enum Direction { LEFT, RIGHT, UP, DOWN};
    Direction snake_dir_;

    //texture ids
    unsigned int head_up_tex_id_;
    unsigned int head_down_tex_id_;
    unsigned int head_right_tex_id_;
    unsigned int head_left_tex_id_;
    unsigned int digits_[10];

    // bool popped_;

    //pointers to image data
    unsigned char* image_snake_head_;
    unsigned char* image_apple_;

};


#endif