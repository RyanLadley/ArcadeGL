#ifndef SNAKE_H
#define SNAKE_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>

#include "../../headers/shader.h"
#include "../../headers/texture.h"
#include "../../headers/renderer.h"
#include "../../headers/drawable.h"

class Snake: public Drawable{
private:
     int start_length, moves, length;
     Uint32 clock_start, speed;

     glm::vec2 direction;
     std::vector<glm::vec2> body;
     std::vector<glm::vec2>::iterator head, tail;

    //Booleans are ment for movements
    //Move_x detmins if the snake is moving left and right or up and down
    //move_processing makes sure the the move has been rendered to the screen before it can continue;
    bool move_x, move_processing; 

    //Moving the body works by move the "tail" of the snake to the front with new location change
    //The rest of the body remains constant
    void move_body();

public:
     Snake(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    //The funtion move operate on a timer
    //The snake will only move afer enough time has passed
    //Condition statment for this is located within method.
    void move();

    void change_right();
    void change_left();
    void change_up();
    void change_down();
 
    //Function called when snake "eats" object
    //It extends the snake by one tile added on to the end
    void extend();

    //Checks if given coord (tile) is on the snakes body;
    bool is_on_body(glm::vec2 coord);

    //Returns the etire body of the snake
    std::vector<glm::vec2> get_body();

    //Draw the entire snakes body (Locations are in the body vector
    void draw(Renderer* renderer);

};

#endif
