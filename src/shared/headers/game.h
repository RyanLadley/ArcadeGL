#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

#include "shader.h"
#include "texture.h"

class Game{
protected:
    SDL_Window* window;
    ShaderManager* shader_manager;
    TextureManager* texture_manager;

    int screen_width, screen_height;

public: 
    Game(SDL_Window* active_window, int width, int height);

    virtual void play_game() = 0;
};

#endif
