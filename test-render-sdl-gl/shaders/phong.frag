#version 330 core

struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight {
    vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec3 phong_point(PointLight light,
	             Material material,
				 vec3 view_position,
				 vec3 frag_normal,
				 vec3 frag_position)
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// diffuse
	vec3 norm = normalize(frag_normal);
	vec3 light_dir = normalize(light.position - frag_position);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	// specular
	vec3 view_dir = normalize(view_position - frag_position);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = light.specular * (material.specular * spec);
	
	return ambient + diffuse + specular;
}

vec3 phong_directional(DirectionalLight light,
	                   Material material,
					   vec3 view_position,
					   vec3 frag_normal,
					   vec3 frag_position)
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// diffuse
	vec3 norm = normalize(frag_normal);
	vec3 light_dir = normalize(-light.direction);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	// specular
	vec3 view_dir = normalize(view_position - frag_position);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = light.specular * (material.specular * spec);
	
	return ambient + diffuse + specular;
}

out vec4 out_color;

in vec3 f_normal;
in vec3 f_pos;

uniform vec3 view_pos;
uniform Material material;
uniform DirectionalLight directionallight;
uniform PointLight pointlight;

void main()
{
    vec3 total_light = vec3(0.0);

	total_light += phong_point(pointlight, material, view_pos, f_normal, f_pos);
	total_light += phong_directional(directionallight, material, view_pos, f_normal, f_pos);

    out_color = vec4(total_light, 1.0);
} 
