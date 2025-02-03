#version 330 core

in vec3 in_position;
in vec3 in_normal;

out vec3 f_normal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(in_position, 1.0);
	f_normal = in_normal;
}
