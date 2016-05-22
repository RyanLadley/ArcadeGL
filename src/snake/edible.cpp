
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

#include "../headers/shader.h"
#include "../headers/texture.h"
#include "../headers/renderer.h"
#include "../headers/drawable.h"
#include "headers/edible.h"

Edible::Edible(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(new_texture, start_location, start_dimensions){}

void Edible::change_location(glm::vec2 new_location){
    location = new_location;
}

