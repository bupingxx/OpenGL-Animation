#version 330 core
layout(location = 0) in vec3 vertex_model;
layout(location = 1) in vec2 uv_model;
layout(location = 2) in vec3 normal_model;

out vec2 uv;
out vec3 normal_eye;
out vec3 camera_eye;
out vec3 light_eye;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;
uniform vec3 light_world;

void main(){
	gl_Position =  projection * view * model * vec4(vertex_model, 1);

	uv = uv_model;
    	
	// camera
	vec3 vertex_eye = (view * model * vec4(vertex_model, 1)).xyz;
	camera_eye = vec3(0,0,0) - vertex_eye;

	// light
	vec3 light = (view * vec4(light_world, 1)).xyz;
	light_eye = light + camera_eye;
	
	// normal
	normal_eye = (normal * vec3(normal_model)).xyz;
}