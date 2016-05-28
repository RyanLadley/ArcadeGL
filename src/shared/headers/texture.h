#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <SOIL/SOIL.h>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

class TextureManager;

class Texture{
  friend class TextureManager;
private:
    GLuint texture;
    float width, height;
     
public:
    Texture();
    GLuint image_format, internal_format;

    void create(float wide, float high, unsigned char* image);

    GLuint get_texture();
    void bind_texture();
};

class TextureManager{
private:
    std::map<std::string, Texture> textures;
public:
    TextureManager();
    
    Texture get_texture(std::string image_path);
    bool add_texture(std::string image_path, GLboolean alpha);
    
};

#endif
