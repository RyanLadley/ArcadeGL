#include "headers/breakout_drawables.h"

//--------------//
//----Block-----//
//-------------//

Block::Block(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec3 start_color)
  :Drawable(texture, start_location, start_dimensions), color(start_color){}

void Block::draw(Renderer* renderer){
    renderer->draw(texture, location, dimensions, 0.0, color) ;
}

Ball::Ball(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Circle(texture, start_location, start_dimensions), velocity(glm::vec2(0,-5)){}

bool Ball::check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim){
    bool game_over = false;
    //Left Boundry
    if(location.x < boundry_loc.x){
        location.x = boundry_loc.x; 
        velocity.x = -1*velocity.x;
    }
    //Right Boundry
    else{ if(location.x +dimensions.s > boundry_loc.x + boundry_dim.x){
        location.x = boundry_loc.x +boundry_dim.x - dimensions.x;
        velocity.x = -1*velocity.x;
   }
   //Top Boundry
   else{ if(location.y < boundry_loc.y){
        location.y = boundry_loc.y;
        velocity.y = -1*velocity.y;
   }
   //Bottom Boundry
   else{ if(location.y+dimensions.y > boundry_loc.y+boundry_dim.y){
       game_over = true;
   } 
   }
   }
   }
   return game_over;
}

//------------//
//---Ball----//
//----------//
bool Ball::check_blocks(Block& block){
    bool collision = false;
    if(this->intersect(block)){
       glm::vec2 block_loc = block.get_location();
       glm::vec2 block_dim = block.get_dimensions();
       collision = true;
       //Detect which side was hit
       //Ball hit from above block
       if(vertex.y <= block_loc.y){
           velocity.y *= -1;
       }
       //Ball hit from bellow block
       else{ if(vertex.y+radius >= block_loc.y+block_dim.y) {
           velocity.y *= -1;

       }
       //Ball hit from right block
       else{ if(vertex.x < block_loc.x){
           velocity.x *= -1;

       }
       //Ball hit from left of block
       else{ if(vertex.x > block_loc.x){
           velocity.x *= -1;
       }

       //It didnt hit a side!!
       else{
           std::cout << "A collision was detected, but no side hit. What gives?" <<std::endl;
       }
       }
       }
       }
    }
    return collision;
}

void Ball::check_paddle(Drawable paddle){
    if(this->intersect(paddle)){
        glm::vec2 paddle_loc = paddle.get_location();
        glm::vec2 paddle_dim = paddle.get_dimensions();
        //Calulate istance from center of paddle
        GLfloat distance = vertex.x - (paddle_loc.x + ((paddle_dim.x/2)));
        GLfloat percentage = distance/(paddle_dim.x);
    
        //Calculate new velocity
        glm::vec2 prev_velocity = this->velocity;
        velocity.x =  3 * percentage;
        velocity.y = -1*abs(velocity.y);
        
        //Normalize to maintain constant speed
       // velocity = glm::normalize(velocity)*glm::length(prev_velocity);                         
   }
}

void Ball::change_location(glm::vec2 new_loc){
    location = new_loc;
    update_vertex();
}


void Ball::move(){
    location.x += velocity.x;
    location.y += velocity.y;
    update_vertex();
}


void Paddle::move_right(){
    location.x +=3;
}

void Paddle::move_left(){
    location.x -=3;
}

void Paddle::move_ball(){
    if( ball != nullptr){
        glm::vec2 ball_loc = ball->get_location();
        glm::vec2 ball_dim = ball->get_dimensions();
        glm::vec2 new_loc(
            location.x +dimensions.x/2 - ball_dim.x/2,
            location.y - ball_dim.x
        );
        ball->change_location(new_loc);
    }
}

//----------------//
//----Paddle------//
//---------------//
Paddle::Paddle(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(texture, start_location, start_dimensions), 
   right_pressed(false), left_pressed(false), ball(nullptr){}

void Paddle::move(){
    if(right_pressed){
        move_right();
    }
    else{ if (left_pressed){
        move_left();
    }
    }
    move_ball();
}

void Paddle::check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim){
    if(location.x < boundry_loc.x){
        location.x = boundry_loc.x;
    }
    else{if(location.x+dimensions.x > boundry_loc.x+boundry_dim.x){
        location.x = boundry_loc.x+boundry_dim.x - dimensions.x;
    }
    }
}


void Paddle::attatch_ball(Ball* new_ball){
    ball = new_ball;
}

void Paddle::detach_ball(){
    ball = nullptr;
}

void Paddle::press_right(bool status){
    right_pressed = status;
}

void Paddle::press_left(bool status){
    left_pressed = status;
}
