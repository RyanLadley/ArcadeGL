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
#include "src/snake/headers/snake_drawable.h"
#include "src/snake/headers/edible.h"
#include "src/snake/headers/snake_game.h"
//Complie with command: g++ -std=c++11 snake.cpp src/snake/snake_drawable.cpp src/snake/edible src/shader.cpp src/texture.cpp src/renderer.cpp src/drawable.cpp -o openGL -lSDL2 -lGLEW -lGL -lSOIL
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
             
    SnakeGame* snake_game = new SnakeGame(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    snake_game->play_game();
 

 
    SDL_GL_DeleteContext(context);  
    SDL_Quit();
    return 0;
}
