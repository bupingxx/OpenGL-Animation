#version 330 core
layout(location = 0) in vec3 vertex_model;
layout(location = 1) in vec2 uv_model;

out vec2 TexCoords;


void main()
{
       TexCoords = uv_model;
        gl_Position = vec4(vertex_model, 1.0);
}