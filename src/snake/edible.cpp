#include "headers/edible.h"

Edible::Edible(Texture new_texture, glm::vec2 start_location, glm::vec2 start_dimensions)
  :Drawable(new_texture, start_location, start_dimensions){}

void Edible::change_location(glm::vec2 new_location){
    location = new_location;
}

