#include "game.hpp"

/*
Initializes the game object to the given width and height.
Parameters:
    win_width (const unsigned int): The width of game's window.
    win_height (const unsigned int): The height of the game's window.
*/
Game::Game(const unsigned int win_width, const unsigned int win_height): 
    play_button_selected_(true),
    in_menu_(true), //TODO change back to true
    window_width_(win_width),
    window_height_(win_height),
    score_area_height_(100),
    entity_width_(32.0f),
    velocity_(4.0f), //4
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

Game::~Game()
{
    delete image_square_;
    delete image_rectangle_;
    delete image_digit_;
}



//render the game
void Game::render_game()
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

        

        //render snake body
        for(int i = 0; i < snake_body_locX_.size(); ++i)
        {
            activate_texture(7, snake_body_locX_[i], snake_body_locY_[i]);
            glDrawArrays(GL_QUADS, 0, 4);
        }
        

        //move snake and check for collision
        move_snake();        
    }

}



//whether play button is selected
bool Game::play_button_selected()
{
    return play_button_selected_;
}

//whether in menu
bool Game::in_menu()
{
    return in_menu_;
}

//change play button selected to opposite
void Game::change_play_button_selected()
{
    play_button_selected_ = !play_button_selected_;
}

//change in_menu to opposite
void Game::change_in_menu()
{
    in_menu_ = !in_menu_;
}

//resets game parameters to default
void Game::reset_game()
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

    reset_timer();
    x_prev_ = x_;
    y_prev_ = y_;

}

//handle user input
void Game::process_input(GLFWwindow* window, const int key, const int action)
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
    if((in_menu() && key == GLFW_KEY_UP && action == GLFW_PRESS) || (in_menu() && key == GLFW_KEY_DOWN && action == GLFW_PRESS))
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



//put a random x and y value within the screen inside x and y
void Game::random_location(float& x, float& y)
{
    srand(time(NULL));
    x = rand() % (window_width_-150) + 100; //between 100 and 650
    y = rand() % (window_height_-150-score_area_height_) + 100; //between 100 and 400
}

//checks whether the snake is colliding with something and processes it
void Game::process_collision()
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
        if(score_ > 99) score_ = 99; //can't display more than 2 digits of score
        length_+=score_/10+1; //the higher the score, the faster snake grows

        //change apple's location
        random_location(x_apple_, y_apple_);
        return;
    }

    //check collision with snake's body
    for(int i = 1; i < snake_body_locX_.size() ; ++i)
    {
        const float body_x = snake_body_locX_[i];
        const float body_y = snake_body_locY_[i];
        
        //x collisions for head's left and right side
        const float l = 8.0f; //leeway for collision
        const bool x_left_corners = x_ >= body_x+l && x_+l <= body_x+entity_width_;
        const bool x_right_corners = x_+entity_width_ >= body_x+l && x_+entity_width_+l <= body_x+entity_width_;
        //y collisions for head's top and bottom side
        const bool y_bottom_corners = y_ >= body_y+l && y_+l <= body_y+entity_width_;
        const bool y_top_corners = y_+entity_width_ >= body_y+l && y_+entity_width_+l <= body_y+entity_width_;
        //if any combination of x and y conditions are true, then there is a collision
        if(x_left_corners && y_bottom_corners 
            || x_left_corners && y_top_corners
            || x_right_corners && y_bottom_corners
            || x_right_corners && y_top_corners)
        {   
            process_game_over();
            return;
        }
    }


}

//handles when the player loses
void Game::process_game_over()
{
    //wait a bit
    reset_timer("game_over");
    while(!check_timer("game_over"));
    in_menu_ = true;
}

/*set timer to current time
Parameters:
    which (const string): Which timer to reset
*/
void Game::reset_timer(const std::string usecase)
{
    if(usecase == "move_snake_body" || usecase == "game_over")
    {
        timer_ = std::chrono::high_resolution_clock::now();
    }else if(usecase == "move_snake")
    {
        timer_move_ = std::chrono::high_resolution_clock::now();
    }else{
        timer_ = std::chrono::high_resolution_clock::now();
        timer_move_ = std::chrono::high_resolution_clock::now();
    }
}

/*
Says whether enough time has passed.
Parameters:
    usecase (std::string): Use case for which it should be checked if enough time has passed
Returns:
    bool: Whether enough time has passed.
*/
bool Game::check_timer(std::string usecase)
{
    std::chrono::duration<float, std::milli> time_diff = std::chrono::high_resolution_clock::now() - timer_;

    if(usecase == "move_snake_body")
    {
        return time_diff.count () > 240.0f; //110, 200
    }
    
    if(usecase == "game_over")
    {
        return time_diff.count () > 1000.0f;
    }

    if(usecase == "move_snake")
    {
        std::chrono::duration<float, std::milli> time_diff_move = std::chrono::high_resolution_clock::now() - timer_move_;
        return time_diff_move.count () > 25.0f;
    }

    return false;
}


/*
Updates snake head's texture for rotation effect
Parameters:
    tex_id (const unsigned int): The id of the texture to use.
*/
void Game::rotate_head(const unsigned int tex_id)
{
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_id);  
}

//loads all the textures needed for the game and menu
void Game::load_all_textures()
{
    //stores snake head image
    constexpr unsigned int imDims = 32;
    image_square_ = new unsigned char[imDims*imDims*4];

    //load snake facing down image and create a texture from it
    load_image("snake_down", image_square_, imDims, imDims);

    //add image to a texture object
    Texture texture_snake_head_down(image_square_, 2, imDims, imDims);
    head_down_tex_id_ = texture_snake_head_down.id();

    //load snake facing right image and create a texture from it
    load_image("snake_right", image_square_, imDims, imDims);
    Texture texture_snake_head_right(image_square_, 2, imDims, imDims);
    head_right_tex_id_ = texture_snake_head_right.id();

    //load snake facing right image and create a texture from it
    load_image("snake_left", image_square_, imDims, imDims);
    //add image to a texture object
    Texture texture_snake_head_left(image_square_, 2, imDims, imDims);
    head_left_tex_id_ = texture_snake_head_left.id();

    //load snake facing right image and create a texture from it
    load_image("snake_body", image_square_, imDims, imDims);
    //add image to a texture object
    Texture texture_snake_body(image_square_, 7, imDims, imDims);

    //load snake facing up image and create a texture from it
    load_image("snake_head", image_square_, imDims, imDims);
    //add image to a texture object
    Texture texture_snake_head_up(image_square_, 2, imDims, imDims);
    head_up_tex_id_ = texture_snake_head_up.id();

    //load apple image and create a texture from it
    load_image("apple", image_square_, imDims, imDims);
    //create apple texture
    Texture texture_apple(image_square_, 3, imDims, imDims);

    //load play button image and create a texture from it
    constexpr unsigned int imWidth = 64;
    constexpr unsigned int imHeight = 32;
    image_rectangle_ = new unsigned char[imWidth*imHeight*4];
    load_image("play", image_rectangle_, imWidth, imHeight);
    Texture texture_play(image_rectangle_, 0, imWidth, imHeight);

    //load quit button image  and create a texture from it
    load_image("quit", image_rectangle_, imWidth, imHeight);
    Texture texture_quit(image_rectangle_, 1, imWidth, imHeight);
    
    //load score image and create a texture from it
    load_image("score", image_rectangle_, imWidth, imHeight);
    Texture texture_score(image_rectangle_, 4, imWidth, imHeight);

    //load digits (0 to 9) images and and create textures from them
    constexpr unsigned int imDigit_width = 4;
    constexpr unsigned int imDigit_height = 8;
    image_digit_ = new unsigned char[imDigit_width*imDigit_height*4];
    //zero
    load_image("zero", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit0(image_digit_, 5, imDigit_width, imDigit_height);
    digits_[0] = texture_digit0.id();
    //one
    load_image("one", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit1(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[1] = texture_digit1.id();
    //two
    load_image("two", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit2(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[2] = texture_digit2.id();
    //three
    load_image("three", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit3(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[3] = texture_digit3.id();
    //four
    load_image("four", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit4(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[4] = texture_digit4.id();
    //five
    load_image("five", image_digit_, imDigit_width, imDigit_height);

    Texture texture_digit5(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[5] = texture_digit5.id();
    //six
    load_image("six", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit6(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[6] = texture_digit6.id();
    //seven
    load_image("seven", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit7(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[7] = texture_digit7.id();
    //eight
    load_image("eight", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit8(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[8] = texture_digit8.id();
    //nine
    load_image("nine", image_digit_, imDigit_width, imDigit_height);
    Texture texture_digit9(image_digit_, 6, imDigit_width, imDigit_height);
    digits_[9] = texture_digit9.id();
    
}


/*
Render the score area and score.
Parameters:
    arrayOffset (unsigned int&): Where the data related to score starts in the vertex buffer
*/
void Game::render_score(unsigned int& arrayOffset)
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

//Update the position of the snake's body 
void Game::move_snake()
{
    //move snake every time interval
    if(check_timer("move_snake"))
    {
        // move snake
        x_ += velocity_x_;
        y_ += velocity_y_;

        //check for collision
        process_collision(); //TODO move up?

        reset_timer("move_snake");
    }
    
    //propegate the prev position of the snake through the body after a period of time
    if(check_timer("move_snake_body"))
    {
        
        //increase queue's length if snake should be larger
        while(length_ > snake_body_locX_.size())
        {
            //add body component outside of screen
            snake_body_locX_.push_back(-100.0f);
            snake_body_locY_.push_back(-100.0f);
        }

        //discard the position at the end of the queue
        if(snake_body_locX_.size() > 0)
        {
            snake_body_locX_.pop_back();
            snake_body_locY_.pop_back();
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
        if(length_ > snake_body_locX_.size()) 
        {
            snake_body_locX_.push_front(x_prev_);
            snake_body_locY_.push_front(y_prev_);
        }


        //set value of prev position to current one
        x_prev_ = x_;
        y_prev_ = y_;

        
        
        //reset timer
        reset_timer("move_snake_body");
    }
    
}

/*
Activates the texture stored at given slot, and sets its position attribute to provided values.
Parameters:
    slot (const unsigned int): The texture to use.
    posX (const float): The x position to set as the texture attribute.
    posY (const float): The y position to set as the texture attribute.
*/
void Game::activate_texture(const unsigned int slot, const float posX, const float posY)
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
void Game::process_up()
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
void Game::process_down()
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
void Game::process_right()
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
void Game::process_left()
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