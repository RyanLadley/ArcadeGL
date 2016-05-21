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

#include "src/headers/shader.h"
#include "src/headers/texture.h"
#include "src/headers/renderer.h"
#include "src/headers/drawable.h"

//Complie with command: g++ -std=c++11 breakout.cpp src/shader.cpp src/texture.cpp src/renderer.cpp src/drawable.cpp -o openGL -lSDL2 -lGLEW -lGL -lSOIL

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class Block: public Drawable{
private:
    glm::vec3 color;
public:
    Block(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec3 start_color)
      :Drawable(texture, start_location, start_dimensions), color(start_color){}
    
    void draw(Renderer* renderer){
        renderer->draw(texture, location, dimensions, 0.0, color) ;
    }
};

class Ball: public Circle{
private:
    glm::vec2 velocity;
    
public:
    Ball(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :Circle(texture, start_location, start_dimensions), velocity(glm::vec2(0,-5)){}

    bool check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim){
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
    
    bool check_blocks(Block& block){
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

    void check_paddle(Drawable paddle){
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

    void change_location(glm::vec2 new_loc){
        location = new_loc;
        update_vertex();
    }
    

    void move(){
        location.x += velocity.x;
        location.y += velocity.y;
        update_vertex();
    }
    
};

class Paddle: public Drawable{
private:
    bool right_pressed = false, left_pressed = false;
    Ball* ball = nullptr;

    void move_right(){
        location.x +=3;
    }

    void move_left(){
        location.x -=3;
    }

    void move_ball(){
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

    
public:
    Paddle(Texture texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :Drawable(texture, start_location, start_dimensions){}

    void move(){
        if(right_pressed){
            move_right();
        }
        else{ if (left_pressed){
            move_left();
        }
        }
        move_ball();
    }

    void check_bounds(glm::vec2 boundry_loc, glm::vec2 boundry_dim){
        if(location.x < boundry_loc.x){
            location.x = boundry_loc.x;
        }
        else{if(location.x+dimensions.x > boundry_loc.x+boundry_dim.x){
            location.x = boundry_loc.x+boundry_dim.x - dimensions.x;
        }
        }
    }


    void attatch_ball(Ball* new_ball){
        ball = new_ball;
    }

    void detach_ball(){
        ball = nullptr;
    }

    void press_right(bool status){
        right_pressed = status;
    }

    void press_left(bool status){
        left_pressed = status;
    }

};
class BreakoutGame{
private:
    SDL_Window* window;
    ShaderManager* shader_manager;
    TextureManager* texture_manager;
    Renderer* renderer;
   
    Tileable* board;
    Tileable* sidebar;
    Border*   border;
    std::vector<Block*> blocks;

    Ball* ball;
    Paddle* paddle;

    bool program_running, game_over;

    void handle_events(SDL_Event* window_event){
            while(SDL_PollEvent(window_event)){
                switch(window_event->type){
                    case SDL_QUIT: 
                       program_running = false;
                       break;
                    case SDL_KEYDOWN:
                        switch (window_event->key.keysym.sym){
                            case SDLK_ESCAPE:
                                program_running = false;
                                break;
                            case SDLK_d:
                                paddle->press_right(true);
                                break;
                            case SDLK_a:
                                paddle->press_left(true);
                                break;
                            case SDLK_SPACE:
                                paddle->detach_ball();
                            
                        }
                        break;
                    case SDL_KEYUP:
                        switch (window_event->key.keysym.sym){
                          case SDLK_d:
                              paddle->press_right(false);
                              break;
                          case SDLK_a:
                              paddle->press_left(false);
                              break;
                      }
                      break;
                    }
            }
    }
    void setup_blocks(){
        glm::vec2 ref = border->get_location();
        glm::vec2 block_dim(50,20); 

        //Top (Green) Row
        for(int i = 60; i < 410;i+=50){
            blocks.push_back(new Block(texture_manager->get_texture("textures/breakout/block.png"), glm::vec2(ref.x +i, ref.y+60), block_dim, glm::vec3(0,1,0)));
        }
        
        //Second (blue) Row
        for(int i = 60; i < 410;i+=50){
            blocks.push_back(new Block(texture_manager->get_texture("textures/breakout/block.png"), glm::vec2(ref.x +i, ref.y+80), block_dim, glm::vec3(0,0,1)));
        }
      
        //Third (red) Row
        for(int i = 60; i < 410;i+=50){
            blocks.push_back(new Block(texture_manager->get_texture("textures/breakout/block.png"), glm::vec2(ref.x +i, ref.y+100), block_dim, glm::vec3(1,0,0)));
        }
       
        //Fourth (purple) Row
        for(int i = 60; i < 410;i+=50){
            blocks.push_back(new Block(texture_manager->get_texture("textures/breakout/block.png"), glm::vec2(ref.x +i, ref.y+120), block_dim, glm::vec3(0.5,0,0.5)));
        }
    }
    void check_collisions(){
        paddle->check_bounds(board->get_location(), board->get_dimensions());            
        
        game_over = ball->check_bounds(board->get_location(), board->get_dimensions());            
        ball->check_paddle(*paddle);
        
        //Check block collision
        for(std::vector<Block*>::iterator it = blocks.begin(); it != blocks.end(); ++it){
            if(ball->check_blocks(*(*it))){
                delete (*it);
                blocks.erase(it);
                break;
            }
        }
    }

    void initiate_textures(){
        texture_manager = new TextureManager();
        texture_manager->add_texture("textures/breakout/board_back.png", false);
        texture_manager->add_texture("textures/breakout/board_border.png", false);
        texture_manager->add_texture("textures/breakout/sidebar.png", false);
        texture_manager->add_texture("textures/breakout/ball.png", true);
        texture_manager->add_texture("textures/breakout/paddle.png", true);
        texture_manager->add_texture("textures/breakout/block.png", false);
    }

    void initiate_drawables(){
         glm::vec2 border_dim = glm::vec2(SCREEN_WIDTH*.75, SCREEN_HEIGHT);
         glm::vec2 tile_dim= glm::vec2(20,20);
         border = new Border(texture_manager->get_texture("textures/breakout/board_border.png"), glm::vec2(SCREEN_WIDTH/4,0), border_dim, tile_dim, tile_dim);  
         sidebar = new Tileable(texture_manager->get_texture("textures/breakout/sidebar.png"), glm::vec2(0,0), glm::vec2(SCREEN_WIDTH/4, SCREEN_HEIGHT), tile_dim);         

         glm::vec2 board_loc = border->get_internal_location();
         glm::vec2 board_dim = border->get_internal_dimensions();
         board = new Tileable(texture_manager->get_texture("textures/breakout/board_back.png"), board_loc, board_dim, tile_dim);  
         
         ball = new Ball(texture_manager->get_texture("textures/breakout/ball.png"), glm::vec2(0,0), glm::vec2(15,15));
         paddle = new Paddle(texture_manager->get_texture("textures/breakout/paddle.png"), glm::vec2(300,400), glm::vec2(60,10));
         paddle->attatch_ball(ball);

         setup_blocks();
    }

    void reset_board(){
        for(std::vector<Block*>::iterator it = blocks.begin(); it != blocks.end(); ++it){
            delete (*it);
            blocks.erase(it);
            it--;
        }
        setup_blocks();
        paddle->attatch_ball(ball);
        game_over = false;
    }

public:
    BreakoutGame(SDL_Window* active_window)
      :window(active_window){
         shader_manager = new ShaderManager();
         shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "breakout");
         renderer = new Renderer(shader_manager->get_shader("breakout"), SCREEN_WIDTH, SCREEN_HEIGHT);

         initiate_textures();
         initiate_drawables();

    }

    void play_game(){
        SDL_Event window_event;
        program_running = true;
        game_over = false;
        while(program_running){
            handle_events(&window_event);
            //Clear Screen
            glClearColor(0,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT);

            ball->move();
            paddle->move();
            
            check_collisions();
            
            sidebar->draw(renderer);
            board->draw(renderer);
            border->draw(renderer);
            
            ball->draw(renderer);           
            paddle->draw(renderer);
            for(std::vector<Block*>::iterator it = blocks.begin(); it != blocks.end(); it++){
                (*it)->draw(renderer);
            }
 
            if(game_over || blocks.begin() == blocks.end()){
                reset_board();
            }           
            SDL_GL_SwapWindow(window);
        }
    } 

};

void setup_glew(){
    glewExperimental = true;     // This flag helps prevent errors with modern OpenGL
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK){
      printf("glew init error\n%s\n", glewGetErrorString( glewError) );
    }

     if( !GLEW_VERSION_2_1 )
     {
         printf( "OpenGL 2.1 not supported!\n" );
     }

    //2. Get OpenGL Version String and GLSL Version string
    const GLubyte * p = glGetString(GL_VERSION); 
    std::cout << "Graphics Driver: " << p << std::endl;

    const GLubyte * q = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout <<  "GLSL Version: " << q << std::endl;

    //3. Check for specific functionality
    if (GLEW_ARB_vertex_array_object){
      printf("genVertexArrays supported\n");
    }
    if (GLEW_APPLE_vertex_array_object){
      printf("genVertexArrayAPPLE supported\n");
    }
}


int main(int argx, char** argv){
    //Initialize Ranadom Seed
    srand(time(NULL));
  
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr<< "SDL_Init Error: " << SDL_GetError() <<std::endl;
        return 1;
    }

    //Create SDL Window
    SDL_Window *window = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if(window == nullptr){
        std::cerr <<"SDL_CreateWindow Error: "<< SDL_GetError() <<std::endl;
        SDL_Quit();
        return 1;
    }


    //Create OpenGL Context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    //Glew handles interaction with graphics drivers
    setup_glew();

    //Initialize openGL"
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
             
    BreakoutGame* breakout_game = new BreakoutGame(window);
    breakout_game->play_game();
 

 
    SDL_GL_DeleteContext(context);  
    SDL_Quit();
    return 0;
}

