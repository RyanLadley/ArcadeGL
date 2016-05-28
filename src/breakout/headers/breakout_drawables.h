#ifndef BREAKOUT_DRAWABLES_H
#define BREAKOUT_DRAWABLES_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "../../shared/headers/shader.h"
#include "../../shared/headers/texture.h"
#include "../../shared/headers/renderer.h"
#include "../../shared/headers/drawable.h"


//Blocks are the objects the ball hits and destroys
class Block: public Drawable{
private:
    glm::vec3 color;
public:
    Block(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec3 start_color);

    //Uses the "color" object as a filter when drawn    
    void draw(Renderer* renderer);
};

//The Ball class is (brace yourself" the ball
class Ball: public Circle{
private:
    //Velocity is the x and y speed and direction in which the ball is moving
    glm::vec2 velocity;
    
public:
    Ball(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    //Checks if ball will "bounce" off the walls
    //Will return true if it passes the bottom wall to indicate a "game_over"
    bool check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim);
    
    //Checks if ball will break a block
    bool check_blocks(Block& block);

    //Check if the ball will bounce off the paddle
    void check_paddle(Drawable paddle);

    //Change location changes the balls location to the vec coord provided
    //Used when ball is attached to paddle
    void change_location(glm::vec2 new_loc);
    
    //Moves the ball according to the current velocity
    void move();
    
};

class Paddle: public Drawable{
private:
    //Used to tell the paddle that the user is pressing a direction key
    bool right_pressed, left_pressed;
   
    //Used if a ball is attached to the paddle
    //nullable
    Ball* ball;


    //Preformed in move() method if right_pressed == true
    void move_right();

    //Preforemed in move() method if left_pressed === ture
    void move_left();

    //If a ball is attached to the paddle, move it with the paddle
    void move_ball();

public:
    Paddle(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    //Called everyframe
    //Moves paddle if user inputs
    void move();

    //Stops the padd from leaving game area
    void check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim);

    //Ataches ball to paddle 
    void attatch_ball(Ball* new_ball);

    //Detaches ball, causing it (the ball) to move on its own terms
    void detach_ball();

    //Called inside of the event handler
    //Changes right_pressed to true if button is pressed
    // and false when it is released
    void press_right(bool status);

    //Called inside of the event handler
    //Changes left_pressed to true if button is pressed
    // and false when it is released
    void press_left(bool status);
};

#endif
