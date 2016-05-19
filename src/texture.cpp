#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <SOIL/SOIL.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "headers/texture.h"

Texture::Texture()
  :image_format(GL_RGB), internal_format(GL_RGB){
    glGenTextures(1, &this->texture);
}

void Texture::create(float wide, float high, unsigned char* image){
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

GLuint Texture::get_texture(){
    return texture;
}

void Texture::bind_texture(){
    glBindTexture(GL_TEXTURE_2D,texture); 
}

//-----------------------//
//----Texture Manager----//
//----------------------//

TextureManager::TextureManager(){}

Texture  TextureManager::get_texture(std::string image_path){
    return textures[image_path];
}


bool TextureManager::add_texture(std::string image_path, GLboolean alpha){
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
