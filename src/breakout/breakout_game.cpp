#include "headers/breakout_game.h"

void BreakoutGame::handle_events(SDL_Event* window_event){
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
void BreakoutGame::setup_blocks(){
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
void BreakoutGame::check_collisions(){
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

void BreakoutGame::initiate_textures(){
    texture_manager->add_texture("textures/breakout/board_back.png", false);
    texture_manager->add_texture("textures/breakout/board_border.png", false);
    texture_manager->add_texture("textures/breakout/sidebar.png", false);
    texture_manager->add_texture("textures/breakout/ball.png", true);
    texture_manager->add_texture("textures/breakout/paddle.png", true);
    texture_manager->add_texture("textures/breakout/block.png", false);
}

void BreakoutGame::initiate_drawables(){
     glm::vec2 border_dim = glm::vec2(screen_width*.75, screen_height);
     glm::vec2 tile_dim= glm::vec2(20,20);
     border = new Border(texture_manager->get_texture("textures/breakout/board_border.png"), glm::vec2(screen_width/4,0), border_dim, tile_dim, tile_dim);  
     sidebar = new Tileable(texture_manager->get_texture("textures/breakout/sidebar.png"), glm::vec2(0,0), glm::vec2(screen_width/4, screen_height), tile_dim);         

     glm::vec2 board_loc = border->get_internal_location();
     glm::vec2 board_dim = border->get_internal_dimensions();
     board = new Tileable(texture_manager->get_texture("textures/breakout/board_back.png"), board_loc, board_dim, tile_dim);  
     
     ball = new Ball(texture_manager->get_texture("textures/breakout/ball.png"), glm::vec2(0,0), glm::vec2(15,15));
     paddle = new Paddle(texture_manager->get_texture("textures/breakout/paddle.png"), glm::vec2(300,400), glm::vec2(60,10));
     paddle->attatch_ball(ball);

     setup_blocks();
}

void BreakoutGame::reset_board(){
    for(std::vector<Block*>::iterator it = blocks.begin(); it != blocks.end(); ++it){
        delete (*it);
        blocks.erase(it);
        it--;
    }
    setup_blocks();
    paddle->attatch_ball(ball);
    game_over = false;
}

BreakoutGame::BreakoutGame(SDL_Window* active_window, int width, int height)
  :Game(active_window, width, height){
     shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "breakout");
     renderer = new Renderer(shader_manager->get_shader("breakout"), screen_width, screen_height);

     initiate_textures();
     initiate_drawables();

}

void BreakoutGame::play_game(){
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

