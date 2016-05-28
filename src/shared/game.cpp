
#include "headers/game.h"

Game::Game(SDL_Window* active_window, int width, int height)
  :window(active_window), screen_width(width), screen_height(height){
    shader_manager = new ShaderManager();
    texture_manager = new TextureManager();
}

