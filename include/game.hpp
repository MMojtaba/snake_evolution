#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <deque>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <chrono>
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
    Game(const unsigned int win_width, const unsigned int win_height);

    ~Game();

    //Not movale or copyable
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    //render the game
    void render_game();
    

    //whether play button is selected
    bool play_button_selected();

    //whether in menu
    bool in_menu();

    //change play button selected to opposite
    void change_play_button_selected();

    //change in_menu to opposite
    void change_in_menu();

    //resets game parameters to default
    void reset_game();

    /*handle user input
    Parameters:
        window (GLFWwindow*): GLFW window the game is running in.
        key (const int): The key that was pressed.
        action (const int): The action that was done on that key (such as press, release, etc)
    */
    void process_input(GLFWwindow* window, const int key, const int action);

    

private:

    //put a random x and y value within the screen inside x and y
    void random_location(float& x, float& y);

    //checks whether the snake is colliding with something and processes it
    void process_collision();

    //handles when the player loses
    void process_game_over();

    /*
    Updates snake head's texture for rotation effect
    Parameters:
        tex_id (const unsigned int): The id of the texture to use.
    */
    void rotate_head(const unsigned int tex_id);

    //loads all the textures needed for the game and menu
    void load_all_textures();

    /*
    Render the score area and score.
    Parameters:
        arrayOffset (unsigned int&): Where the data related to score starts in the vertex buffer
    */
    void render_score(unsigned int& arrayOffset);

    //Update the position of the snake's body 
    void move_snake_body();

    /*
    Activates the texture stored at given slot, and sets its position attribute to provided values.
    Parameters:
        slot (const unsigned int): The texture to use.
        posX (const float): The x position to set as the texture attribute.
        posY (const float): The y position to set as the texture attribute.
    */
    void activate_texture(const unsigned int slot, const float posX, const float posY);

    //process user pressing up arrow key
    void process_up();

    //process user pressing down arrow key
    void process_down();

    //process user pressing right arrow key
    void process_right();


    //process user pressing left arrow key
    void process_left();


    const float entity_width_; //width of single gameplay entities (snake head, apple)
    const unsigned int window_width_; //game window's width
    const unsigned int window_height_; //game window's height
    const unsigned int score_area_height_; //score area height


    bool play_button_selected_; //whether the play button is selected in the mneu
    bool in_menu_; //whether the use is in the menu
    int score_; //user's score
    // clock_t timer_; //used as a timer to update snake's body location
    std::chrono::high_resolution_clock::time_point timer_;

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