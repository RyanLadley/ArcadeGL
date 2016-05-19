
#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "texture.h"
#include "renderer.h"
class Drawable{
protected:
    glm::vec2 location;
    glm::vec2 dimensions;

    Texture texture; 
     
public:
     Drawable(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions);
     
     virtual void draw(Renderer *renderer);
     
     //Returns Top left corner of Drawable
     virtual glm::vec2 get_location();
  
     virtual glm::vec2 get_dimensions();
};

class Tileable: public Drawable{
protected:
    glm::vec2 tile_dimensions;
public:
     Tileable(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim);
     
     virtual void draw(Renderer *renderer);
};

class Border: public Tileable{
private:
    glm::vec2 border_thickness;

public:
     Border(Texture new_texture,glm::vec2 start_location, glm::vec2 start_dimensions, glm::vec2 tile_dim, glm::vec2 border_thick);
    
     //Returns top left of inside rectangle
     glm::vec2 get_internal_location();
 
     glm::vec2 get_internal_dimensions();
 
     //TODO this only draws in multiples of the size of the texture
     //Crop texture so fits given dimensions as specified
     void draw(Renderer *renderer);
};
