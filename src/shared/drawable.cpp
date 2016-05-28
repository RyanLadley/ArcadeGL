
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

bool Drawable::intersect(Drawable& object){
    glm::vec2 obj_loc = object.get_location();
    glm::vec2 obj_dim = object.get_dimensions();
    return (location.x < obj_loc.x + obj_dim.x && location.x +dimensions.x > obj_loc.x &&
            location.y < obj_loc.y + obj_dim.y && location.y +dimensions.y > obj_loc.y );
}

//-------------//
//---Circle---//
//-----------//
Circle::Circle(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(new_texture, start_location, start_dimensions){
    radius = start_dimensions.x/2;
    update_vertex();
}

void Circle::update_vertex(){
    vertex.x = location.x + radius;
    vertex.y = location.y + radius;
}

bool Circle::intersect(Drawable& object){
    if(this->Drawable::intersect(object)){
        glm::vec2 obj_loc = object.get_location();
        glm::vec2 obj_dim = object.get_dimensions();
        
        //Calculate expensive circle collision
        //Half dimensions  used a lot, calulate once
        glm::vec2 half_dim (
              obj_dim.x/2,
              obj_dim.y/2
        );

        //Get the center point of the rectangle
        glm::vec2 obj_center(
            obj_loc.x + half_dim.x,
            obj_loc.y + half_dim.y
        );

        //Find distance from center of rect to center of circle
        glm::vec2 distance = vertex - obj_center;

        //Chack Sides
        if (distance.x <= half_dim.x || 
            distance.y <= half_dim.y ){
            return true; 
        }

        //Calculate Corners       
        float a = distance.x - half_dim.x;
        float b = distance.y - half_dim.y;
        float corners_squared = (a*a) + (b*b);

        return corners_squared <= (radius * radius);
    }
    return false;
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
