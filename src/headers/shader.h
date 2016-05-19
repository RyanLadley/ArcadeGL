#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

//Forward declare so Shader can make friend class
class ShaderManager;

class Shader{
  friend class ShaderManager;
     GLuint program;
public:
    GLuint get_program();
    
    void use();
    void compile(const GLchar* vertex_source, const GLchar* fragment_source);
    
    void set_int(const GLchar* name, GLint value);
    void set_vec3(const GLchar* name, glm::vec3 value);
    void set_matrix4(const GLchar* name, glm::mat4 matrix);
};

class ShaderManager{
private:
    std::map<std::string, Shader> shaders;
public:
    ShaderManager();

    Shader get_shader(std::string key);
    
    bool add_shader(std::string vertex_path, std::string fragment_path, std::string key);

};

#endif
