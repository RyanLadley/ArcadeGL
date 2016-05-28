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

#include "src/shared/headers/shader.h"
#include "src/shared/headers/texture.h"
#include "src/shared/headers/renderer.h"
#include "src/shared/headers/drawable.h"
#include "src/shared/headers/game.h"
#include "src/shared/headers/observer.h"

#include "src/snake/headers/snake_drawable.h"
#include "src/snake/headers/edible.h"
#include "src/snake/headers/snake_game.h"

#include "src/breakout/headers/breakout_drawables.h"
#include "src/breakout/headers/breakout_game.h"

#include "src/asteroids/headers/asteroids_drawables.h"
#include "src/asteroids/headers/asteroids_game.h"
//Complie with command: g++ -std=c++11 main.cpp src/snake/snake_game.cpp src/snake/snake_drawable.cpp src/snake/edible.cpp src/breakout/breakout_drawables.cpp src/breakout/breakout_game.cpp src/asteroids/asteroids_drawables.cpp src/asteroids/asteroids_game.cpp src/shared/shader.cpp src/shared/texture.cpp src/shared/renderer.cpp src/shared/drawable.cpp src/shared/game.cpp -o openGL -lSDL2 -lGLEW -lGL -lSOIL
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class Button: public Drawable{
public: 
   Button(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
     :Drawable(new_texture, start_location, start_dimensions) {} 

      bool check_press(int mouse_x, int mouse_y){
          return (location.x < mouse_x && location.x + dimensions.x > mouse_x &&
                  location.y < mouse_y && location.y + dimensions.y > mouse_y );
      }

      virtual void pressed(){
          std::cout<<"Wow, you pressed a button" << std::endl;
      } 
};

class GameButton: public Button{
private:
    Game* game;

public:
    GameButton(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions, Game* new_game)
      :Button(new_texture, start_location, start_dimensions), game(new_game){}

    void pressed(){
        game->play_game();
    }
};

class MainMenu{
private:
     SDL_Window* window;
     int screen_width, screen_height;
     bool menu_running;

     ShaderManager* shader_manager;
     TextureManager* texture_manager;
     Renderer* renderer;
     
     Tileable* background;
     std::vector<Button*> buttons;
    
     void handle_events(SDL_Event* window_event){
            while(SDL_PollEvent(window_event)){
                switch(window_event->type){
                    case SDL_QUIT: 
                       menu_running = false;
                       break;
                    case SDL_KEYDOWN:
                       switch (window_event->key.keysym.sym){
                           case SDLK_ESCAPE:
                               menu_running = false;
                               break;
                       }
                       break;
                    case SDL_MOUSEBUTTONDOWN:
                        int mouse_x, mouse_y;
                        SDL_GetMouseState(&mouse_x, &mouse_y); 
                        press_button(mouse_x, mouse_y);
                        break;                      
                }
            }
    }

    void press_button(int mouse_x, int mouse_y){
        for(std::vector<Button*>::iterator it = buttons.begin(); it != buttons.end(); ++it){
            if((*it)->check_press(mouse_x, mouse_y)){
                (*it)->pressed();
            }
        }
    }

    void initiate_drawables(){
         background = new Tileable(texture_manager->get_texture("textures/main/background.png"), glm::vec2(0,0),glm::vec2(screen_width, screen_height), glm::vec2(40,40));
         glm::vec2 button_dim(180,180);
         //Snake Button
         buttons.push_back(new GameButton(texture_manager-> get_texture("textures/main/snake_button.png"), glm::vec2(40,40), button_dim, new SnakeGame(window, screen_width, screen_height)));
         //Breakout Button
         buttons.push_back(new GameButton(texture_manager-> get_texture("textures/main/breakout_button.png"), glm::vec2(40,280), button_dim, new BreakoutGame(window, screen_width, screen_height)));
         //Asteroids Button
         buttons.push_back(new GameButton(texture_manager-> get_texture("textures/main/asteroids_button.png"), glm::vec2(420,40), button_dim, new AsteroidsGame(window, screen_width, screen_height)));
         //Construction Button
         buttons.push_back(new Button(texture_manager-> get_texture("textures/main/under_con.png"), glm::vec2(420,280), button_dim));
     }

     void initiate_textures(){
        texture_manager = new TextureManager();
     
        texture_manager->add_texture("textures/main/background.png", false);
        texture_manager->add_texture("textures/main/snake_button.png", false);
        texture_manager->add_texture("textures/main/breakout_button.png", false);
        texture_manager->add_texture("textures/main/asteroids_button.png",false);
        texture_manager->add_texture("textures/main/under_con.png",false);
    }
public:
    MainMenu(SDL_Window* active_window, int width, int height)
      :window(active_window), screen_width(width), screen_height(height), menu_running(false){
         shader_manager = new ShaderManager();
         shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "main");
         renderer = new Renderer(shader_manager->get_shader("main"), screen_width, screen_height);

         initiate_textures();
         initiate_drawables();
    }

    void display(){
        SDL_Event window_event;
        menu_running = true;
        while(menu_running){
            handle_events(&window_event);
            //Clear Screen
            glClearColor(0,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT);
        
            background->draw(renderer);
            for(std::vector<Button*>::iterator it = buttons.begin(); it != buttons.end(); ++it){
                (*it)->draw(renderer);
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
             
    MainMenu* menu = new MainMenu(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    menu->display();
 
    SDL_GL_DeleteContext(context);  
    SDL_Quit();
    return 0;
}
