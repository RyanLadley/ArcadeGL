#ifndef RENDERER_H
#define RENDERER_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "shader.h"
#include "texture.h"
class Renderer{
private:
    Shader shader;
    const int SCREEN_WIDTH, SCREEN_HEIGHT;

public:
    Renderer(Shader new_shader, int screen_width, int screen_height);
 
    void draw(Texture texture, glm::vec2 position, glm::vec2 dimensions, GLfloat rotate, glm::vec3 color);
    void init_renderer();
};

#endif
