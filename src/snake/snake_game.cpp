#include "headers/snake_game.h"

void SnakeGame::handle_events(SDL_Event* window_event){
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
void SnakeGame::move_edible(){
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
bool SnakeGame::check_boundry(){
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
bool SnakeGame::check_cannibal(){
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
bool SnakeGame::check_death(){
    return check_boundry() || check_cannibal();
}

//Checks if snake eats the edible 
void SnakeGame::snake_eats(){
    glm::vec2 snake_location = snake->get_location(),
              edible_location = apple->get_location();

    if(edible_location.x == snake_location.x &&
       edible_location.y == snake_location.y ){
        snake->extend();
        move_edible();
    }
}
void SnakeGame::reset(){
    delete snake;
    snake = new Snake(texture_manager->get_texture("textures/snake/snake.png"),start_location , tile_dim);
    game_over = false;
    move_edible();
}

 SnakeGame::SnakeGame(SDL_Window* active_window, int width, int height)
   :Game(active_window,width, height){
     shader_manager->add_shader("shaders/vertex.glsl", "shaders/fragment.glsl", "thing");
     
     texture_manager->add_texture("textures/snake/board_back.png", false); 
     texture_manager->add_texture("textures/snake/board_border.png", false); 
     texture_manager->add_texture("textures/snake/snake.png", false); 
     texture_manager->add_texture("textures/snake/apple.png",true);         
     texture_manager->add_texture("textures/snake/sidebar.png",false);         

     glm::vec2 border_dim = glm::vec2(screen_width*.75, screen_height);
     tile_dim= glm::vec2(20,20);
     border = new Border(texture_manager->get_texture("textures/snake/board_border.png"), glm::vec2(screen_width/4,0), border_dim, tile_dim, glm::vec2(40,40));  
     sidebar = new Tileable(texture_manager->get_texture("textures/snake/sidebar.png"), glm::vec2(0,0), glm::vec2(screen_width/4, screen_height), tile_dim);         

     glm::vec2 board_loc = border->get_internal_location();
     glm::vec2 board_dim = border->get_internal_dimensions();
     board = new Tileable(texture_manager->get_texture("textures/snake/board_back.png"), board_loc, board_dim, tile_dim);  
     

     //Place snake in center of board
     start_location = glm::vec2(board_loc.x+(board_dim.x)/2, board_loc.y +(board_dim.y)/2);   
     snake = new Snake(texture_manager->get_texture("textures/snake/snake.png"),start_location , tile_dim);

     apple = new Edible(texture_manager->get_texture("textures/snake/apple.png"), start_location, tile_dim);
     move_edible(); 

     renderer = new Renderer(shader_manager->get_shader("thing"), screen_width, screen_height);
 }

void SnakeGame::play_game(){
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

