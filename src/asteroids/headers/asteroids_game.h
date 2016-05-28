#ifndef ASTEROIDS_GAME_H
#define ASTEROIDS_GAME_H

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

#include "asteroids_drawables.h"
class AsteroidsGame : public Game{
private:
    Renderer* renderer;
    
    Tileable* board;
    Tileable* sidebar;
    Border*   border;

    Rocket* rocket;
    std::vector<Beam*> beams;
    std::vector<Asteroid*> asteroids; 

    bool program_running, game_over;

    void handle_events(SDL_Event* window_event);

    //Limits tehe number of beams out at once to how many were initiated
    void shoot();
    
    void initiate_beams();

    void initiate_textures();


    void initiate_drawables();

    void initiate_asteroids();

    void reset_board();

    void draw_objects();

    void move_beams();

    void move_objects();

    bool check_asteroid_v_rocket();

    void check_asteroid_v_beam();

    void check_boundries();

    void check_collisions();

public:
    AsteroidsGame(SDL_Window* active_window, int width, int height);

    void play_game();

};

#endif
