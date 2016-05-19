
#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "headers/texture.h"
#include "headers/renderer.h"
#include "headers/drawable.h"
//----------------//
//----Drawable----//
//---------------//

Drawable::Drawable(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :texture(new_texture), location(start_location), dimensions(start_dimensions){}

void Drawable::draw(Renderer *renderer){
    renderer->draw(texture, location, dimensions, 0.0, glm::vec3(1,1,1)) ;
}  

//Returns Top left corner of Drawable
glm::vec2 Drawable::get_location(){
    return location;    
} 

glm::vec2 Drawable::get_dimensions(){
    return dimensions;    
}

//-----------------//
//----Tileable-----//
//----------------//
 
Tileable::Tileable(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim)
  :Drawable(new_texture, start_location, start_dimensions), tile_dimensions(tile_dim){}

void Tileable::draw(Renderer *renderer){
    glm::vec2 tile;
    //Rows draw
    for(tile.y = location.y; tile.y < location.y + dimensions.y; tile.y += tile_dimensions.y){
        //Columns Draw
        for(tile.x = location.x; tile.x < location.x+dimensions.x; tile.x += tile_dimensions.x){
            renderer->draw(texture,tile, tile_dimensions, 0.0, glm::vec3(1,1,1));
        }
    }
}


//--------------//
//----Border----//
//--------------//

Border::Border(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim, glm::vec2 border_thick)
  :Tileable(new_texture,start_location, start_dimensions, tile_dim), border_thickness(border_thick){}

//Returns top left of inside rectangle
glm::vec2 Border::get_internal_location(){
    return location+border_thickness;
}

glm::vec2 Border::get_internal_dimensions(){
    return glm::vec2(dimensions.x - 2*border_thickness.x, dimensions.y - 2*border_thickness.y);
}

//TODO this only draws in multiples of the size of the texture
//Crop texture so fits given dimensions as specified
void Border::draw(Renderer *renderer){
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
