#ifndef EDIBLE_H
#define EDIBLE_H

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

class Edible: public Drawable{
public:
     Edible(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);
     
     //Moves edible to specified location;
     //usually called when edible is eaten or a new game begins
     void change_location(glm::vec2 new_location);

};

#endif
