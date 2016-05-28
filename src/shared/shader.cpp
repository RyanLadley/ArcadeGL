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

#include "headers/shader.h"


void Shader::use(){
    glUseProgram(program);
}

GLuint Shader::get_program(){
    return program;
}

void Shader:: compile(const GLchar* vertex_source, const GLchar* fragment_source){
    GLint shader_status;   
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if (shader_status == GL_FALSE){
        std::cout <<"ERROR: There was an error with vertexShader"<< std::endl;
    }    

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader); 
   
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if (shader_status == GL_FALSE){
        std::cout <<"ERROR: There was an error with fragmentShader"<< std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader); 
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
}

void Shader::set_int(const GLchar* name, GLint value){
    glUniform1i(glGetUniformLocation(program, name), value);
}

void Shader::set_vec3(const GLchar* name, glm::vec3 value){
    glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
}
void Shader::set_matrix4(const GLchar* name, glm::mat4 matrix){
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

//-----------------------------//
//-------Shader Manager-------//
//----------------------------//
ShaderManager::ShaderManager(){}

Shader ShaderManager::get_shader(std::string key){
    return shaders[key];
}
bool ShaderManager::add_shader(std::string vertex_path, std::string fragment_path, std::string key){
    bool success = true;
    std::string vertex_source, fragment_source;

    //Load source code from path
    try{
        std::ifstream vertex_file(vertex_path);
        std::ifstream fragment_file(fragment_path);
       
        std::stringstream vertex_stream, fragment_stream;
        vertex_stream << vertex_file.rdbuf();
        fragment_stream <<fragment_file.rdbuf();
        
        vertex_file.close();
        fragment_file.close();
        
        vertex_source = vertex_stream.str();
        fragment_source = fragment_stream.str();
    }
    catch (std::exception e){
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
        success = false;
    }
    const GLchar* vertex_code = vertex_source.c_str();
    const GLchar* fragment_code = fragment_source.c_str();
    
    //Create Shader from new sources
    Shader shader;
    shader.compile(vertex_code, fragment_code);
    success = shaders.emplace(key,shader).second && success;
    return success;
}
