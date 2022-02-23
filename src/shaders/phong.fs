#version 330 core
in vec2 uv;
in vec3 normal_eye;
in vec3 camera_eye;
in vec3 light_eye;

out vec3 color;

uniform sampler2D TextureSampler;
uniform vec3 light_mat;
uniform vec3 ambient_mat;
uniform vec3 diffuse_mat;
uniform vec3 specular_mat;
uniform float specular_power;

void main(){
    // ambient 
    vec3 ambient = ambient_mat;

	// diffuse
	vec3 n = normalize(normal_eye); 
	vec3 l = normalize(light_eye); 
    float diffuse_angle = clamp(dot(n, l), 0, 1);
	vec3 diffuse = diffuse_mat * light_mat * diffuse_angle;

    // specular
	vec3 e = normalize(camera_eye); 
	vec3 r = reflect(-l, n); 
    float specular_angle = max(clamp(dot(e, r), 0, 1), 0);
	vec3 specular = specular_mat * light_mat * pow(specular_angle, specular_power);
    
	//color = texture( TextureSampler, uv ).rgb; 
	color = (ambient + diffuse + specular) * texture( TextureSampler, uv ).rgb; 
}