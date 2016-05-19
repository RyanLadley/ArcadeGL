#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
//Complie with command: g++ -std=c++11 snake.cpp -o openGL -lSDL2 -lGLEW -lGL -lSOIL
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class Shader{
private:
     GLuint program;
public:
    Shader(){};
    
    void use(){
        glUseProgram(program);
    }

    GLuint get_program(){
        return program;
    }

    void compile(const GLchar* vertex_source, const GLchar* fragment_source){
        GLint shader_status;   
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_source, NULL);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
        if (shader_status == GL_FALSE){
            std::cout <<"ERROR: There was an error with vertexShader"<< std::endl;
        }    
    
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_source, NULL);
        glCompileShader(fragment_shader); 
       
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
        if (shader_status == GL_FALSE){
            std::cout <<"ERROR: There was an error with fragmentShader"<< std::endl;
        }

        program = glCreateProgram();
        glAttachShader(program, vertex_shader); 
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
    }

    void set_int(const GLchar* name, GLint value){
        glUniform1i(glGetUniformLocation(program, name), value);
    }

    void set_vec3(const GLchar* name, glm::vec3 value){
        glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
    }
    void set_matrix4(const GLchar* name, glm::mat4 matrix){
        glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
};

class Texture{
private:
    GLuint texture;
    float width, height;
     
public:
    GLuint image_format, internal_format;
    Texture()
      :image_format(GL_RGB), internal_format(GL_RGB){
        glGenTextures(1, &this->texture);
    }

    void create(float wide, float high, unsigned char* image){
        this->width = wide;
        this->height = high;
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, this->internal_format, this->width, this->height, 0, this->image_format, GL_UNSIGNED_BYTE, image); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint get_texture(){
        return texture;
    }

    void bind_texture(){
        glBindTexture(GL_TEXTURE_2D,texture); 
    }
};

class ResourceManager{
private:
    std::map<std::string, Shader> shaders;
    std::map<std::string, Texture> textures;
public:
    ResourceManager(){}

    Shader get_shader(std::string key){
        return shaders[key];
    }

    Texture get_texture(std::string image_path){
        return textures[image_path];
    }

    bool add_shader(std::string vertex_path, std::string fragment_path, std::string key){
        bool success = true;
        std::string vertex_source, fragment_source;
  
        //Load source code from path
        try{
            std::ifstream vertex_file(vertex_path);
            std::ifstream fragment_file(fragment_path);
           
            std::stringstream vertex_stream, fragment_stream;
            vertex_stream << vertex_file.rdbuf();
            fragment_stream <<fragment_file.rdbuf();
            
            vertex_file.close();
            fragment_file.close();
            
            vertex_source = vertex_stream.str();
            fragment_source = fragment_stream.str();
        }
        catch (std::exception e){
            std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
            success = false;
        }
        const GLchar* vertex_code = vertex_source.c_str();
        const GLchar* fragment_code = fragment_source.c_str();
        
        //Create Shader from new sources
        Shader shader;
        shader.compile(vertex_code, fragment_code);
        success = shaders.emplace(key,shader).second && success;
        return success;
    }

    bool add_texture(std::string image_path, GLboolean alpha){
        Texture texture;
        if(alpha){
            texture.internal_format = GL_RGBA;
            texture.image_format = GL_RGBA;
        }
        int width, height;
        unsigned char* image = SOIL_load_image(image_path.c_str(), &width, &height, 0, texture.image_format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
        if(image == nullptr){
            std::cout<< "ERROR: Failed to read texture" << image_path << std::endl;
        }
        texture.create(width, height, image);
        SOIL_free_image_data(image);
        return textures.emplace(image_path, texture).second;
    }
};

class Renderer{
private:
    Shader shader;


public:
    Renderer(Shader new_shader)
      :shader(new_shader){
        init_renderer();
    }
 
    void draw(Texture texture, glm::vec2 position, glm::vec2 dimensions, GLfloat rotate, glm::vec3 color){
        this->shader.use();
       
        glm::mat4 model;
        //Reposition 
        model = glm::translate(model, glm::vec3(position, 0.0f)); 
        //Rotation around center (Radians)
        model = glm::translate(model, glm::vec3(0.5f * dimensions.x, 0.5f * dimensions.y, 0.0f));
        model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); 
        model = glm::translate(model, glm::vec3(-0.5f * dimensions.x, -0.5f * dimensions.y, 0.0f)); 
        //Get proper scale
        model = glm::scale(model, glm::vec3(dimensions, 1.0f));
        //Push model to shder
        this->shader.set_matrix4("model", model);

        //add color
        this->shader.set_vec3("color", color);

        texture.bind_texture();
         
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
    }

    void init_renderer(){
        //Textures are all squares
        GLfloat vertices[]={
         // x     y       r     g     b      Texture Coords
            0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
            1.0f, 1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
            1.0f, 0.0f,   0.5f, 0.5f, 0.0f,    1.0f, 0.0f,
            0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f
        };

        //VertexBuffer
        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //Makes vertex_buffer active array buffer

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //Element Array(order of which the vertices will be drawn)
        GLuint elements[] = {
            0, 1, 2,
            0, 3, 2
        };

        GLuint element_buffer;
        glGenBuffers(1, &element_buffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        //Shaders
        shader.use();

        //Shader Formating
        GLint pos_attrib = glGetAttribLocation(shader.get_program(), "position");
        glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
        glEnableVertexAttribArray(pos_attrib);
    
        GLint color_attrib = glGetAttribLocation(shader.get_program(), "color");
        glVertexAttribPointer(color_attrib,3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));  
        glEnableVertexAttribArray(color_attrib); 

        GLint texture_attrib = glGetAttribLocation(shader.get_program(), "tex_coord");
        glVertexAttribPointer(texture_attrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(texture_attrib);

        //(0,0) as upper right
        glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f); 
        shader.set_matrix4("projection", projection);
        
        //Initiate texture;
        shader.set_int("image",0);
    }

};

class Drawable{
protected:
    glm::vec2 location;
    glm::vec2 dimensions;

    Texture texture; 
    //Transformation Variables
     
public:
     Drawable(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
       :texture(new_texture), location(start_location), dimensions(start_dimensions){}
     
    virtual void draw(Renderer *renderer){
         renderer->draw(texture, location, dimensions, 0.0, glm::vec3(1,1,1)) ;
     }  
     
     //Returns Top left corner of Drawable
     virtual glm::vec2 get_location(){
         return location;    
     } 
  
     virtual glm::vec2 get_dimensions(){
         return dimensions;    
     }   
};

class Tileable: public Drawable{
protected:
    glm::vec2 tile_dimensions;
public:
     Tileable(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim)
       :Drawable(new_texture, start_location, start_dimensions), tile_dimensions(tile_dim){}
 
     virtual void draw(Renderer *renderer){
         glm::vec2 tile;
         //Rows draw
         for(tile.y = location.y; tile.y < location.y + dimensions.y; tile.y += tile_dimensions.y){
             //Columns Draw
             for(tile.x = location.x; tile.x < location.x+dimensions.x; tile.x += tile_dimensions.x){
                 renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
             }
         }
     }
};

class Border: public Tileable{
private:
    glm::vec2 border_thickness;

public:
     Border(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim, glm::vec2 border_thick)
       :Tileable(new_texture,start_location, start_dimensions, tile_dim), border_thickness(border_thick){}
    
     //Returns top left of inside rectangle
     glm::vec2 get_internal_location(){
         return location+border_thickness;
     }
 
     glm::vec2 get_internal_dimensions(){
         return glm::vec2(dimensions.x - 2*border_thickness.x, dimensions.y - 2*border_thickness.y);
     }
 
     //TODO this only draws in multiples of the size of the texture
     //Crop texture so fits given dimensions as specified
     void draw(Renderer *renderer){
         glm::vec2 start_location;
         glm::vec2 tile;
         
         //Draw Left
         //Columns draw
         for(tile.y = location.y; tile.y < location.y + dimensions.y; tile.y += tile_dimensions.y){
             //Rows Draw
             for(tile.x = location.x; tile.x < location.x+border_thickness.x; tile.x += tile_dimensions.x){
                 renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
             }
         }

         //Draw Right
         //Columns draw
         for(tile.y = location.y; tile.y < location.y + dimensions.y; tile.y += tile_dimensions.y){
             //Rows Draw
             for(tile.x = location.x+dimensions.x - border_thickness.x; tile.x < location.x+dimensions.x; tile.x += tile_dimensions.x){
                 renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
             }
         }

         //Draw Top
         //Row draw
         for(tile.x = location.x+border_thickness.x; tile.x < location.x + dimensions.x; tile.x += tile_dimensions.x){
             //Columns Draw
             for(tile.y = location.y; tile.y < location.y+border_thickness.y; tile.y += tile_dimensions.y){
                 renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
             }
         }

         //Drawbotom 
         //Rows draw
         for(tile.x = location.x+border_thickness.x; tile.x < location.x + dimensions.x; tile.x += tile_dimensions.x){
             //Columns Draw
             for(tile.y = location.y+dimensions.y-border_thickness.y; tile.y < location.y+dimensions.y; tile.y += tile_dimensions.y){
                 renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
             }
         }

     }
};
class Edible: public Drawable{
public:
     Edible(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
       :Drawable(new_texture, start_location, start_dimensions){}

     void change_location(glm::vec2 new_location){
         location = new_location;
     }

};

class Snake: public Drawable{
private:
     int move_timer = 0, start_length = 4, moves = 0, length = 4;
     Uint32 clock_start = 0, speed = 1100;

     glm::vec2 direction;
     std::vector<glm::vec2> body;
     std::vector<glm::vec2>::iterator head, tail;

    //Booleans are ment for movements
    //Move_x detmins if the snake is moving left and right or up and down
    //move_processing makes sure the the move has been rendered to the screen before it can continue;
    bool move_x = false, move_processing = false; 

    //Moving the body works by move the "tail" of the snake to the front with new location change
    //The rest of the body remains constant
    void move_body(){
        (*tail) = location;
        head = tail;
 
        //since the old tail is now the head, we find to new tail
        if(tail ==  body.begin()){
            tail = body.end();
        }
        tail--;
    }

public:
     Snake(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
       :Drawable(new_texture, start_location, start_dimensions){
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
    void move(){
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

    void change_right(){
        if(!move_x && !move_processing){
            move_x = true;
            move_processing = true;
            direction = glm::vec2(dimensions.x, 0);
            
        }
    }
    void change_left(){
        if(!move_x && !move_processing){
            move_x = true;
            move_processing = true;
            direction = glm::vec2(-1*dimensions.x, 0);;
        }
    }
    void change_up(){
        if(move_x && !move_processing){
            move_x = false;
            move_processing = true;
            direction = glm::vec2(0, -1*dimensions.y);
        }
    }
    void change_down(){
        if(move_x && !move_processing){
            move_x = false;
            move_processing = true;
            direction = glm::vec2(0, dimensions.y);
        }
    }
 
    //Function called when snake "eats" opject
    void extend(){
        tail = body.insert(tail, (*tail))+1;
        speed*=.95;
    }

    //Checks if given coord is on the snakes body;
    bool is_on_body(glm::vec2 coord){
       for(std::vector<glm::vec2>::iterator it = body.begin(); it != body.end(); it++){
           if( it->x == coord.x && it->y == coord.y){
               return true; 
           }
       }
       return false;
    }
 
    std::vector<glm::vec2> get_body(){
        return body;
    }

    //Draw the entire snakes body (Locations are in the body vector
    void draw(Renderer* renderer){
        for(std::vector<glm::vec2>::iterator it = body.begin(); it != body.end(); ++it){
            renderer->draw(texture,(*it), dimensions, 0.0, glm::vec3(1,1,1));
        }
    }

};

class SnakeGame{
private:
    ResourceManager* manager;
    SDL_Window* window;
    Renderer* renderer;

    bool program_running;
    bool game_over;
    Tileable* board;
    Tileable* sidebar;
    Border* border;
    Snake* snake;
    Edible* apple;

    glm::vec2 tile_dim;
    glm::vec2 start_location; 
 
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
                               snake->change_right();
                               break;
                           case SDLK_a:
                               snake->change_left();
                               break;
                           case SDLK_s:
                               snake->change_down();
                               break;
                           case SDLK_w:
                               snake->change_up();
                               break;
                           
                       }
                       break;
                }
            }
    }
    //Moves Edible to random location on board
    void move_edible(){
        //There are board.w/tile_size posible x's on playing board
        //There are iboard.h/tile_size posible y's on playing board
        glm::vec2 board_loc = board->get_location();
        glm::vec2 board_dim= board->get_dimensions();
        int x_possible=board_dim.x/tile_dim.x, y_possible = board_dim.y/tile_dim.y;
      
        //Get new location, we pick one of the pissible locations by random
        //This is then mulipled by the tile_size to get the actual(offset) board location
        //The offset is then removed by adding it to the boards start possition 
        glm::vec2 new_location;    
        do{
            new_location.x = rand() % x_possible*tile_dim.x + board_loc.x;
            new_location.y = rand() % y_possible*tile_dim.y + board_loc.y;
        }while(snake->is_on_body(new_location));//Check to make sure edilbe is not located on snake body    
        apple->change_location(new_location);  
    }

    //Checks if snake has left board
    bool check_boundry(){
         glm::vec2 snake_location = snake->get_location();
         glm::vec2 boundry_loc = board->get_location();
         glm::vec2 boundry_dim = board->get_dimensions();

         if(snake_location.x >= boundry_loc.x + boundry_dim.x || snake_location.x < boundry_loc.x ||
            snake_location.y >= boundry_loc.y + boundry_dim.y || snake_location.y < boundry_loc.y ){
             return true;
         }
         return false;
    }
    //Chacks if snake is eating itself
    bool check_cannibal(){
        std::vector<glm::vec2> body = snake->get_body();
        glm::vec2 snake_location = snake->get_location();
        
        //The head is in a changing place within the body vector;
        //This means that for an intersection with the body to occur,
        //Two itterations must have the location of the head;
        bool head_found = false; 
        
        for(std::vector<glm::vec2>::iterator it = body.begin(); it != body.end(); it++){
            if( it->x == snake_location.x && it->y == snake_location.y){
                if(head_found){
                    return true;
                }
                else{
                   head_found = true;
                }
            }
        }
        return false;
    }

    //Combines Both boundry and self eating into one function
    //Returns true if either are true
    bool check_death(){
        return check_boundry() || check_cannibal();
    }
 
    //Checks if snake eats the edible 
    void snake_eats(){
        glm::vec2 snake_location = snake->get_location(),
                  edible_location = apple->get_location();
    
        if(edible_location.x == snake_location.x &&
           edible_location.y == snake_location.y ){
            snake->extend();
            move_edible();
        }
    }
    void reset(){
        delete snake;
        snake = new Snake(manager->get_texture("textures/snake/snake.png"),start_location , tile_dim);
        game_over = false;
        move_edible();
    }
public:
     SnakeGame(SDL_Window* active_window)
       :window(active_window){
         manager = new ResourceManager();
         manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "thing");
         manager->add_texture("textures/rocket.png", true); 
         manager->add_texture("textures/snake/board_back.png", false); 
         manager->add_texture("textures/snake/board_border.png", false); 
         manager->add_texture("textures/snake/snake.png", false); 
         manager->add_texture("textures/snake/apple.png",true);         
         manager->add_texture("textures/snake/sidebar.png",false);         

         glm::vec2 border_dim = glm::vec2(SCREEN_WIDTH*.75, SCREEN_HEIGHT);
         tile_dim= glm::vec2(20,20);
         border = new Border(manager->get_texture("textures/snake/board_border.png"), glm::vec2(SCREEN_WIDTH/4,0), border_dim, tile_dim, glm::vec2(40,40));  
         sidebar = new Tileable(manager->get_texture("textures/snake/sidebar.png"), glm::vec2(0,0), glm::vec2(SCREEN_WIDTH/4, SCREEN_HEIGHT), tile_dim);         

         glm::vec2 board_loc = border->get_internal_location();
         glm::vec2 board_dim = border->get_internal_dimensions();
         board = new Tileable(manager->get_texture("textures/snake/board_back.png"), board_loc, board_dim, tile_dim);  
         

         //Place snake in center of board
         start_location = glm::vec2(board_loc.x+(board_dim.x)/2, board_loc.y +(board_dim.y)/2);   
         snake = new Snake(manager->get_texture("textures/snake/snake.png"),start_location , tile_dim);

         apple = new Edible(manager->get_texture("textures/snake/apple.png"), start_location, tile_dim);
         move_edible(); 

         renderer = new Renderer(manager->get_shader("thing"));
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
        
            snake->move();
            snake_eats();

            sidebar->draw(renderer);
            board->draw(renderer);
            border->draw(renderer);
            snake->draw(renderer);
            apple->draw(renderer);

            SDL_GL_SwapWindow(window);
            if(check_death()){
                reset();
            }
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
             
    SnakeGame* snake_game = new SnakeGame(window);
    snake_game->play_game();
 

 
    SDL_GL_DeleteContext(context);  
    SDL_Quit();
    return 0;
}
