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

#include "../../headers/shader.h"
#include "../../headers/texture.h"
#include "../../headers/renderer.h"
#include "../../headers/drawable.h"

class Edible: public Drawable{
public:
     Edible(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);

     void change_location(glm::vec2 new_location);

};

#endif
