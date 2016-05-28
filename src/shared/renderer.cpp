#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "headers/renderer.h"
#include "headers/shader.h"
#include "headers/texture.h"

Renderer::Renderer(Shader new_shader, int screen_width, int screen_height)
  :shader(new_shader), SCREEN_WIDTH(screen_width), SCREEN_HEIGHT(screen_height){
    init_renderer();
}

void Renderer::draw(Texture texture, glm::vec2 position, glm::vec2 dimensions, GLfloat rotate, glm::vec3 color){
    this->shader.use();
   
    glm::mat4 model;
    //Reposition 
    model = glm::translate(model, glm::vec3(position, 0.0f)); 
    //Rotation around center (Radians)
    model = glm::translate(model, glm::vec3(0.5f * dimensions.x, 0.5f * dimensions.y, 0.0f));
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); 
    model = glm::translate(model, glm::vec3(-0.5f * dimensions.x, -0.5f * dimensions.y, 0.0f)); 
    //Get proper scale
    model = glm::scale(model, glm::vec3(dimensions, 1.0f));
    //Push model to shder
    this->shader.set_matrix4("model", model);

    //add color
    this->shader.set_vec3("color", color);

    texture.bind_texture();
     
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
}

void Renderer::init_renderer(){
    //Textures are all squares
    GLfloat vertices[]={
     // x     y       r     g     b      Texture Coords
        0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        1.0f, 1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        1.0f, 0.0f,   0.5f, 0.5f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f
    };

    //VertexBuffer
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //Makes vertex_buffer active array buffer

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Element Array(order of which the vertices will be drawn)
    GLuint elements[] = {
        0, 1, 2,
        0, 3, 2
    };

    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //Shaders
    shader.use();

    //Shader Formating
    GLint pos_attrib = glGetAttribLocation(shader.get_program(), "position");
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
    glEnableVertexAttribArray(pos_attrib);

    GLint color_attrib = glGetAttribLocation(shader.get_program(), "color");
    glVertexAttribPointer(color_attrib,3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));  
    glEnableVertexAttribArray(color_attrib); 

    GLint texture_attrib = glGetAttribLocation(shader.get_program(), "tex_coord");
    glVertexAttribPointer(texture_attrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(texture_attrib);

    //(0,0) as upper right
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f); 
    shader.set_matrix4("projection", projection);
    
    //Initiate texture;
    shader.set_int("image",0);
}
