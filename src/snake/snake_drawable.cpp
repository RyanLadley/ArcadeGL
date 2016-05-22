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
#include "headers/snake_drawable.h"

//Moving the body works by move the "tail" of the snake to the front with new location change
//The rest of the body remains constant
void Snake::move_body(){
    (*tail) = location;
    head = tail;

    //since the old tail is now the head, we find to new tail
    if(tail ==  body.begin()){
        tail = body.end();
    }
    tail--;
}

Snake::Snake(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(new_texture, start_location, start_dimensions), start_length(4), moves(0), 
   speed(1100), move_x(false), move_processing(false){
     clock_start = SDL_GetTicks();
     direction = glm::vec2(0, -1*dimensions.y);
     //Fill Snakes body with values
     body.push_back(location);
     head = body.begin();
     tail = body.end() - 1;
}

//The funtion move operate on a timer
//The snake will only move afer enough time has passed
//Condition statment for this is located within method.
void Snake::move(){
    if(SDL_GetTicks()-clock_start >= speed){
        location += direction;
        move_processing = false;
        //Handle First few moves
        if(moves < start_length){
            extend();
        }
        move_body();
        clock_start = SDL_GetTicks();
        moves++;
    }
}

void Snake::change_right(){
    if(!move_x && !move_processing){
        move_x = true;
        move_processing = true;
        direction = glm::vec2(dimensions.x, 0);
        
    }
}
void Snake::change_left(){
    if(!move_x && !move_processing){
        move_x = true;
        move_processing = true;
        direction = glm::vec2(-1*dimensions.x, 0);;
    }
}
void Snake::change_up(){
    if(move_x && !move_processing){
        move_x = false;
        move_processing = true;
        direction = glm::vec2(0, -1*dimensions.y);
    }
}
void Snake::change_down(){
    if(move_x && !move_processing){
        move_x = false;
        move_processing = true;
        direction = glm::vec2(0, dimensions.y);
    }
}

//Function called when snake "eats" opject
void Snake::extend(){
    tail = body.insert(tail, (*tail))+1;
    speed*=.95;
}

//Checks if given coord is on the snakes body;
bool Snake::is_on_body(glm::vec2 coord){
   for(std::vector<glm::vec2>::iterator it = body.begin(); it != body.end(); it++){
       if( it->x == coord.x && it->y == coord.y){
           return true; 
       }
   }
   return false;
}

std::vector<glm::vec2> Snake::get_body(){
    return body;
}

//Draw the entire snakes body (Locations are in the body vector
void Snake::draw(Renderer* renderer){
    for(std::vector<glm::vec2>::iterator it = body.begin(); it != body.end(); ++it){
        renderer->draw(texture,(*it), dimensions, 0.0, glm::vec3(1,1,1));
    }
}
