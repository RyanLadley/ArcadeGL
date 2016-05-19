#version 150 core

uniform vec3 color;
in vec2 texture_coordinate;

uniform sampler2D image;

out vec4 outColor;

void main(){
    outColor = texture(image, texture_coordinate)*glm::vec4(color,1.0);
}
