#version 150 core

in vec2 position;
in vec3 color;
in vec2 tex_coord;

out vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 projection;

void main(){
    texture_coordinate = tex_coord;
    gl_Position = projection*model*vec4(position, 0, 1.0);
}

