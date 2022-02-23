#version 330 core
layout(location = 0) in vec3 vertex_model;
layout(location = 1) in vec2 uv_model;
layout(location = 2) in vec3 normal_model;


out vec2 TexCoords;
out vec3 v_eye;
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(model * vec4(vertex_model, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * normal_model;
    vs_out.TexCoords = uv_model;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(vertex_model, 1.0);
    v_eye = vec4(vertex_model, 1).xyz;
}