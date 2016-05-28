#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

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

#include "snake_drawable.h"
#include "edible.h"
class SnakeGame: public Game{
private:
    Renderer* renderer;

    bool program_running;
    bool game_over;
    Tileable* board;
    Tileable* sidebar;
    Border* border;
    Snake* snake;
    Edible* apple;

    glm::vec2 tile_dim;
    glm::vec2 start_location;
    
    void handle_events(SDL_Event* window_event);
    
    //Moves Edible to random location on board
    void move_edible();
    

    //Checks if snake has left board
    bool check_boundry();
    
    //Chacks if snake is eating itself
    bool check_cannibal();

    //Combines Both boundry and self eating into one function
    //Returns true if either are true
    bool check_death();
 
    //Checks if snake eats the edible 
    void snake_eats();
    
    //Ressts board to starting state
    void reset();

public:
     SnakeGame(SDL_Window* active_window, int width, int height);

    void play_game();
};

#endif
