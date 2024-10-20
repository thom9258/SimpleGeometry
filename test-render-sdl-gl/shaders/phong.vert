#version 330 core

in vec3 v_pos;
in vec3 v_normal;

out vec3 f_normal;
out vec3 f_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	f_pos = vec3(model * vec4(v_pos, 1.0));
	//f_normal = v_normal;
	f_normal = mat3(transpose(inverse(model))) * v_normal;
    gl_Position = proj * view * vec4(f_pos, 1.0);
}
