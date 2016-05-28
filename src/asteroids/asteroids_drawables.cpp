#include "headers/asteroids_drawables.h"

//----------------------//
//---AsteroidsObject---//
//---------------------//

AsteroidsObject::AsteroidsObject(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(new_texture, start_location, start_dimensions){}

//When boundry is hit, the rocket is transported to oposite side of the screen
void AsteroidsObject::check_boundry(Drawable& boundry){
    glm::vec2 boundry_loc = boundry.get_location();
    glm::vec2 boundry_dim = boundry.get_dimensions();

    //offset is used to make sure the rocket is completly off screen before and after transport
    float offset = std::max(dimensions.x, dimensions.y);
   
    //Right Side
    if(location.x < boundry_loc.x-offset){
        location.x = boundry_loc.x + boundry_dim.x;
    }
    //Left Side
    else{ if( location.x> boundry_loc.x + boundry_dim.x + offset){
        location.x = boundry_loc.x - offset;
    }
    //Top
    else{ if(location.y < boundry_loc.y - offset){
        location.y = boundry_loc.y + boundry_dim.y + offset;
    }
    //Bottom
    else{ if(location.y > boundry_loc.y + boundry_dim.y + offset){
        location.y = boundry_loc.y - offset;
    }  
    }
    }
    }
   
}
//-----------------//
//-----Rocket------//
//----------------//

void Rocket::thrust(){
    //Why is this backwords?
    speed.x += .1 * sin(direction);
    speed.y += -0.1 * cos(direction);
    //Caps speed
    if(glm::length(speed) > 2){
        speed = glm::normalize(speed)*max_speed;
    }
}

void Rocket::rotate_right(){
    if(rotation_accel < 0.1)
        rotation_accel += 0.005;
}

void Rocket::rotate_left(){
    if(rotation_accel > -0.1)
    rotation_accel -= 0.005;
}
void Rocket::accelerate(){
    if(forward_pressed)
        thrust();
    if(right_pressed)
        rotate_right();
    if(left_pressed)
        rotate_left();
}

Rocket::Rocket(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :AsteroidsObject(new_texture, start_location, start_dimensions), initial_location(start_location), direction(0), speed(glm::vec2(0,0)), max_speed(glm::vec2(2,2)),
  rotation_accel(0), right_pressed(false), left_pressed(false), forward_pressed(false){}

void Rocket::move(){
    accelerate();
    direction += rotation_accel;
    location += speed;
}

void Rocket::draw(Renderer *renderer){
    renderer->draw(texture, location, dimensions, direction, glm::vec3(1,1,1)) ;
}  

void Rocket::press_forward(bool status){
    forward_pressed = status;
}
void Rocket::press_right(bool status){
    right_pressed = status;
}
void Rocket::press_left(bool status){
    left_pressed = status;
}

float Rocket::get_direction(){
    return direction;
}

void Rocket::reset(){
    rotation_accel = 0;
    direction = 0; 
    speed= glm::vec2(0,0);  
    location = initial_location;
}

//------------//
//---Beam----//
//----------//

bool Beam::check_life(){
   return SDL_GetTicks() - life_start < life_span;
}

Beam::Beam(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :AsteroidsObject(new_texture, start_location, start_dimensions), direction(0), life_span(2000), alive(false),
   life_start(0), speed(glm::vec2(0,0)){}

bool Beam::is_alive(){
    return alive;
}

void Beam::kill(){
    alive = false;
}

void Beam::fire_beam(Rocket* rocket){
    direction = rocket->get_direction();
    location = rocket->get_location();
    speed = glm::vec2(
        1 * sin(direction),
       -1 * cos(direction)
    );
    life_start = SDL_GetTicks();
    alive = true; 
}

void Beam::move(){
   alive = check_life();
   if(alive){
       location+=speed;
   }
}

void Beam::draw(Renderer* renderer){
    if(alive){
        renderer->draw(texture, location, dimensions, direction, glm::vec3(1,1,1));
    }
}

//---------------//
//---Asteroid---//
//--------------//

Asteroid::Asteroid(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :AsteroidsObject(new_texture, start_location, start_dimensions){
    direction = rand() % (6);
    speed = glm::vec2(
        1 * sin(direction),
       -1 * cos(direction)
    );
}

void Asteroid::move(){
    location += speed;
}
