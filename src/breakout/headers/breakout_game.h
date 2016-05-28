#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

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
#include "../../shared/headers/game.h"

#include "breakout_drawables.h"

class BreakoutGame: public Game{
private:
    Renderer* renderer;
   
    //Background and border images
    Tileable* board;
    Tileable* sidebar;
    Border*   border;

    //Game Objects
    std::vector<Block*> blocks;
    Ball* ball;
    Paddle* paddle;

    bool program_running, game_over;

    //Method to handle all user inputs
    void handle_events(SDL_Event* window_event);

    //Creates all blocks for the the game.
    //Each Row has a different color
    //Called inside of initiate_drawables()
    void setup_blocks();
 
    //calls all the collision methods for the objects of the game
    void check_collisions();
  
    //adds images to texture object to be used on drawables
    void initiate_textures();
  
    //Creates all drawables for the game
    void initiate_drawables();

    //Puts board to it's initial condition
    void reset_board();

public:
    BreakoutGame(SDL_Window* active_window, int width, int height);

    //Where the game logic is called
    void play_game();
};

#endif
