#version 330 core

in vec3 f_normal;

out vec4 out_color;

void main()
{
	out_color = vec4(f_normal * 0.5 + 0.5, 1.0);
} 
