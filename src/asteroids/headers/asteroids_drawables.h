#ifndef ASTEROIDS_DRAWABLES_H
#define ASTEROIDS_DRAWABLES_H

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

class Rocket;

//Covers all objcets within the game
//Created because all objects teleport to other side of map the boundry is reached
class AsteroidsObject: public Drawable{
public:
    AsteroidsObject(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    //When boundry is hit, the rocket is transported to oposite side of the screen
    void check_boundry(Drawable& boundry);
};


//The Rocket is the user!
class Rocket: public AsteroidsObject{
private:
    //direction is in radians
    float direction, rotation_accel; 
    glm::vec2 speed, max_speed, initial_location;
    
    //Tells Rocket the current user inpout
    bool right_pressed, left_pressed, forward_pressed;

    //Next three methods are used within accelerate method
    //Thrust is forword movement, roate_mthods change the direction the rocket faces
    void thrust();
    void rotate_right();
    void rotate_left();
    
    //Calls appropriate method of the thrust and rotations given a user input
    void accelerate();

public:
    Rocket(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);
    
    //All movements are called here everyframe
    void move();
 
    //Firection is pst to renderer to rotate image
    void draw(Renderer *renderer);

    //Determine User inputs
    void press_forward(bool status);
    void press_right(bool status);
    void press_left(bool status);

    //Returns the current direction of rocket in radians
    float get_direction();
   
    //sets rocket to initial stets 
    void reset();
};

class Beam: public AsteroidsObject{
private:
    //Used to determin how long obect remains on the board
    float life_span, life_start,
          direction;
    glm::vec2 speed;
    bool alive;
 
    //Checks the time the bbeam has been alive. IF its time (life_span) has been reached, the object dies
    bool check_life();

public:
    Beam(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    //Retursn "alive" boolean
    bool is_alive();
 
    //Kill simply makes alive false, so as far as other game object are concered, the beam no longer exsists
    void kill();
 
    //Creates beam to move in the direction of the given rocket
    //It is "fired" from the rocket
    void fire_beam(Rocket* rocket);

    //Moves the beam in it "direction" varaible
    void move();
  
    void draw(Renderer* renderer);
};

class Asteroid: public AsteroidsObject{
private:
    glm::vec2 speed;
    float direction;

public:
    Asteroid(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);

    void move();

};

#endif
