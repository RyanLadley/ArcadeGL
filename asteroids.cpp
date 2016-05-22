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

//Complie with command: g++ -std=c++11 asteroids.cpp src/shader.cpp src/texture.cpp src/renderer.cpp src/drawable.cpp -o openGL -lSDL2 -lGLEW -lGL -lSOIL

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float PI = 3.141592; 

class Rocket;

class AsteroidsObject: public Drawable{
public:
    AsteroidsObject(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :Drawable(new_texture, start_location, start_dimensions){}

    //When boundry is hit, the rocket is transported to oposite side of the screen
    void check_boundry(Drawable& boundry){
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
};


class Rocket: public AsteroidsObject{
private:
    //direction is in radians
    float direction, rotation_accel; 
    glm::vec2 speed, max_speed, initial_location;
    

    bool right_pressed, left_pressed, forward_pressed;

    void thrust(){
        //Why is this backwords?
        speed.x += .1 * sin(direction);
        speed.y += -0.1 * cos(direction);
        //Caps speed
        if(glm::length(speed) > 2){
            speed = glm::normalize(speed)*max_speed;
        }
    }

    void rotate_right(){
        if(rotation_accel < 0.1)
            rotation_accel += 0.005;
    }

    void rotate_left(){
        if(rotation_accel > -0.1)
        rotation_accel -= 0.005;
    }
    void accelerate(){
        if(forward_pressed)
            thrust();
        if(right_pressed)
            rotate_right();
        if(left_pressed)
            rotate_left();
    }


public:
    Rocket(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :AsteroidsObject(new_texture, start_location, start_dimensions), initial_location(start_location), direction(0), speed(glm::vec2(0,0)), max_speed(glm::vec2(2,2)),
      rotation_accel(0), right_pressed(false), left_pressed(false), forward_pressed(false){}
    
    void move(){
        accelerate();
        direction += rotation_accel;
        location += speed;
    }
 
    void draw(Renderer *renderer){
        renderer->draw(texture, location, dimensions, direction, glm::vec3(1,1,1)) ;
    }  

    void press_forward(bool status){
        forward_pressed = status;
    }
    void press_right(bool status){
        right_pressed = status;
    }
    void press_left(bool status){
        left_pressed = status;
    }

    float get_direction(){
        return direction;
    }
    
    void reset(){
        rotation_accel = 0;
        direction = 0; 
        speed= glm::vec2(0,0);  
        location = initial_location;
    }
};

class Beam: public AsteroidsObject{
private:
    //Used to determin how long obect remains on the board
    float life_span, life_start,
          direction;
    glm::vec2 speed;
    bool alive;
 
    bool check_life(){
       return SDL_GetTicks() - life_start < life_span;
    }

public:
    Beam(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :AsteroidsObject(new_texture, start_location, start_dimensions), direction(0), life_span(2000), alive(false),
       life_start(0), speed(glm::vec2(0,0)){}

    bool is_alive(){
        return alive;
    }
 
    void kill(){
        alive = false;
    }
 
    void fire_beam(Rocket* rocket){
        direction = rocket->get_direction();
        location = rocket->get_location();
        speed = glm::vec2(
            1 * sin(direction),
           -1 * cos(direction)
        );
        life_start = SDL_GetTicks();
        alive = true; 
    }

    void move(){
       alive = check_life();
       if(alive){
           location+=speed;
       }
    }
  
    void draw(Renderer* renderer){
        if(alive){
            renderer->draw(texture, location, dimensions, direction, glm::vec3(1,1,1));
        }
    }
};

class Asteroid: public AsteroidsObject{
private:
    glm::vec2 speed;
    float direction;

public:
    Asteroid(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
      :AsteroidsObject(new_texture, start_location, start_dimensions){
        direction = rand() % (6);
        speed = glm::vec2(
            1 * sin(direction),
           -1 * cos(direction)
        );
    }

    void move(){
        location += speed;
    }

};

class AsteroidsGame{
private:
    SDL_Window* window;
    ShaderManager* shader_manager;
    TextureManager* texture_manager;
    Renderer* renderer;
    
    Tileable* board;
    Tileable* sidebar;
    Border*   border;

    Rocket* rocket;
    std::vector<Beam*> beams;
    std::vector<Asteroid*> asteroids; 

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
                                rocket->press_right(true);
                                break;
                            case SDLK_a:
                                rocket->press_left(true);
                                break;
                            case SDLK_w:
                                rocket->press_forward(true);
                                break;
                            case SDLK_SPACE:
                                shoot();
                                break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch (window_event->key.keysym.sym){
                          case SDLK_d:
                              rocket->press_right(false);
                              break;
                          case SDLK_a:
                              rocket->press_left(false);
                              break;
                          case SDLK_w:
                              rocket->press_forward(false);
                              break;
                      }
                      break;
                    }
            }
    }

    //Limits tehe number of beams out at once to how many were initiated
    void shoot(){
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            if(!((*it)->is_alive())){//If beam is not alive
                (*it)->fire_beam(rocket);
                break;
            }
        }
    }
    
    void initiate_beams(){
        for(int i = 0; i <3; i++){
            beams.push_back(new Beam(texture_manager->get_texture("textures/asteroids/beam.png"), glm::vec2(0,0), glm::vec2(2,15)));
        }
    }

    void initiate_textures(){
        texture_manager = new TextureManager();
        texture_manager->add_texture("textures/asteroids/board_back.png", false);
        texture_manager->add_texture("textures/asteroids/board_border.png", false);
        texture_manager->add_texture("textures/asteroids/sidebar.png", false);
    
        texture_manager->add_texture("textures/asteroids/rocket.png", true);
        texture_manager->add_texture("textures/asteroids/beam.png", true);
        texture_manager->add_texture("textures/asteroids/asteroid.png", true);
    }


    void initiate_drawables(){
         glm::vec2 border_dim = glm::vec2(SCREEN_WIDTH*.75, SCREEN_HEIGHT);
         glm::vec2 tile_dim= glm::vec2(20,20);
         border = new Border(texture_manager->get_texture("textures/asteroids/board_border.png"), 
                             glm::vec2(SCREEN_WIDTH/4,0), border_dim, tile_dim, tile_dim);  
         sidebar = new Tileable(texture_manager->get_texture("textures/asteroids/sidebar.png"), 
                                glm::vec2(0,0), glm::vec2(SCREEN_WIDTH/4, SCREEN_HEIGHT), tile_dim);         

         glm::vec2 board_loc = border->get_internal_location();
         glm::vec2 board_dim = border->get_internal_dimensions();
         board = new Tileable(texture_manager->get_texture("textures/asteroids/board_back.png"), board_loc, board_dim, tile_dim);  
         
         glm::vec2 rocket_loc(board_loc.x + board_dim.x/2, board_loc.y + board_dim.y/2);
         rocket = new Rocket(texture_manager->get_texture("textures/asteroids/rocket.png"), rocket_loc, glm::vec2(20,40));
         initiate_asteroids();
    }

    void initiate_asteroids(){
        glm::vec2 board_loc = board->get_location();
        glm::vec2 board_dim = board->get_dimensions();

        glm::vec2 asteroid_dim(60,60);
 
        for(int i = board_loc.y; i < board_loc.y + board_dim.y; i+=200){
            asteroids.push_back(new Asteroid(texture_manager->get_texture("textures/asteroids/asteroid.png"), 
                                glm::vec2(board_loc.x,i), glm::vec2 (60,60)));
            asteroids.push_back(new Asteroid(texture_manager->get_texture("textures/asteroids/asteroid.png"), 
                                glm::vec2(board_loc.x+board_dim.x,i), glm::vec2 (60,60)));
        }
        for(int j = board_loc.x; j < board_loc.x + board_dim.x; j+=200){
            asteroids.push_back(new Asteroid(texture_manager->get_texture("textures/asteroids/asteroid.png"), 
                                glm::vec2(j,board_loc.y), glm::vec2 (60,60)));
            asteroids.push_back(new Asteroid(texture_manager->get_texture("textures/asteroids/asteroid.png"), 
                                glm::vec2(j,board_loc.y+board_dim.y), glm::vec2 (60,60)));
        }
    }

    void reset_board(){
        for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            delete *asteroid;
        }
        asteroids.clear();
        initiate_asteroids();
        rocket->reset();
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            (*it)->kill();
        }
        game_over = false;
    }


    void draw_objects(){
        rocket->draw(renderer);
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            (*it)->draw(renderer);
        }
        for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            (*asteroid)->draw(renderer);
        }
    }

    void move_beams(){
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            if((*it)->is_alive())
                (*it)->move();
        }
    }

    void move_objects(){
        rocket->move();
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            move_beams();
        }
        for(std::vector<Asteroid*>::iterator asteroid= asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            (*asteroid)->move();
        }
    }

    bool check_asteroid_v_rocket(){
        bool collision = false;
        for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            if((*asteroid)->intersect(*rocket)){
                collision = true;
            }
        }
        return collision;
    }

    void check_asteroid_v_beam(){
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            if((*it)->is_alive() && (*it)->intersect(*(*asteroid))){
                    (*it)->kill();
                    delete *asteroid;
                    asteroids.erase(asteroid);
                    break;
                }
            }
        }
    }

    void check_boundries(){
        rocket->check_boundry(*board);
        for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
            (*it)->check_boundry(*board);
        }
        for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
            (*asteroid)->check_boundry(*board);
        }
    }

    void check_collisions(){
        check_boundries();
        check_asteroid_v_beam();
        game_over = check_asteroid_v_rocket();
    }

public:
    AsteroidsGame(SDL_Window* active_window)
      :window(active_window){
         shader_manager = new ShaderManager();
         shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "asteroids");
         renderer = new Renderer(shader_manager->get_shader("asteroids"), SCREEN_WIDTH, SCREEN_HEIGHT);

         initiate_textures();
         initiate_drawables();
         initiate_beams();
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

            board->draw(renderer);
            
            move_objects();
            check_collisions();
            draw_objects();

            sidebar->draw(renderer);
            border->draw(renderer);
            
            if(game_over){
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
             
    AsteroidsGame* asteroids_game = new AsteroidsGame(window);
    asteroids_game->play_game();
 
    SDL_GL_DeleteContext(context);  
    SDL_Quit();
    return 0;
}

