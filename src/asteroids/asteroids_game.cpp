
#include "headers/asteroids_game.h"

const float PI = 3.141592; 

void AsteroidsGame::handle_events(SDL_Event* window_event){
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
void AsteroidsGame::shoot(){
    for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
        if(!((*it)->is_alive())){//If beam is not alive
            (*it)->fire_beam(rocket);
            break;
        }
    }
}

void AsteroidsGame::initiate_beams(){
    for(int i = 0; i <3; i++){
        beams.push_back(new Beam(texture_manager->get_texture("textures/asteroids/beam.png"), glm::vec2(0,0), glm::vec2(2,15)));
    }
}

void AsteroidsGame::initiate_textures(){
    texture_manager->add_texture("textures/asteroids/board_back.png", false);
    texture_manager->add_texture("textures/asteroids/board_border.png", false);
    texture_manager->add_texture("textures/asteroids/sidebar.png", false);

    texture_manager->add_texture("textures/asteroids/rocket.png", true);
    texture_manager->add_texture("textures/asteroids/beam.png", true);
    texture_manager->add_texture("textures/asteroids/asteroid.png", true);
}


void AsteroidsGame::initiate_drawables(){
     glm::vec2 border_dim = glm::vec2(screen_width*.75, screen_height);
     glm::vec2 tile_dim= glm::vec2(20,20);
     border = new Border(texture_manager->get_texture("textures/asteroids/board_border.png"), 
                         glm::vec2(screen_width/4,0), border_dim, tile_dim, tile_dim);  
     sidebar = new Tileable(texture_manager->get_texture("textures/asteroids/sidebar.png"), 
                            glm::vec2(0,0), glm::vec2(screen_width/4, screen_height), tile_dim);         

     glm::vec2 board_loc = border->get_internal_location();
     glm::vec2 board_dim = border->get_internal_dimensions();
     board = new Tileable(texture_manager->get_texture("textures/asteroids/board_back.png"), board_loc, board_dim, tile_dim);  
     
     glm::vec2 rocket_loc(board_loc.x + board_dim.x/2, board_loc.y + board_dim.y/2);
     rocket = new Rocket(texture_manager->get_texture("textures/asteroids/rocket.png"), rocket_loc, glm::vec2(20,40));
     initiate_asteroids();
}

void AsteroidsGame::initiate_asteroids(){
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

void AsteroidsGame::reset_board(){
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


void AsteroidsGame::draw_objects(){
    rocket->draw(renderer);
    for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
        (*it)->draw(renderer);
    }
    for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
        (*asteroid)->draw(renderer);
    }
}

void AsteroidsGame::move_beams(){
    for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
        if((*it)->is_alive())
            (*it)->move();
    }
}

void AsteroidsGame::move_objects(){
    rocket->move();
    for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
        move_beams();
    }
    for(std::vector<Asteroid*>::iterator asteroid= asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
        (*asteroid)->move();
    }
}

bool AsteroidsGame::check_asteroid_v_rocket(){
    bool collision = false;
    for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
        if((*asteroid)->intersect(*rocket)){
            collision = true;
        }
    }
    return collision;
}

void AsteroidsGame::check_asteroid_v_beam(){
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

void AsteroidsGame::check_boundries(){
    rocket->check_boundry(*board);
    for(std::vector<Beam*>::iterator it = beams.begin(); it != beams.end(); ++it){
        (*it)->check_boundry(*board);
    }
    for(std::vector<Asteroid*>::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid){
        (*asteroid)->check_boundry(*board);
    }
}

void AsteroidsGame::check_collisions(){
    check_boundries();
    check_asteroid_v_beam();
    game_over = check_asteroid_v_rocket();
}

AsteroidsGame::AsteroidsGame(SDL_Window* active_window, int width, int height)
  :Game(active_window, width, height){
     shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "asteroids");
     renderer = new Renderer(shader_manager->get_shader("asteroids"), screen_width, screen_height);

     initiate_textures();
     initiate_drawables();
     initiate_beams();
}

void AsteroidsGame::play_game(){
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
