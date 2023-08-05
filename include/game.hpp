#include <GL/glew.h>
#include <vector>


class Game
{
public:
    Game(unsigned int id): 
        play_button_selected_(true),
        in_menu_(true),
        score_(0)
    {
        //initially snake moving upwards
        velocity_[0] = 0;
        velocity_[1] = 0.1;

        //start snake near middle of screen
        position_[0] = 400;
        position_[1] = 250;
    }

    //process user pressing left arrow key
    void process_left()
    {

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
    void render_game(unsigned int program_id)
    {
        //render snake
        

    }


private:
    bool play_button_selected_;
    bool in_menu_;
    float velocity_[2]; //velocity of snake in x and y direction
    float position_[2]; //position of snake
    int score_; //user's score
    // unsigned int id_; //program id associated with the game
};