#version 330 core
layout(location = 0) in vec3 vertex_model;
layout(location = 1) in vec2 uv_model;
layout(location = 2) in vec3 normal_model;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){

		gl_Position = lightSpaceMatrix * model * vec4(vertex_model, 1.0);


}