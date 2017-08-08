#version 400 

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(inPos, 1.0);
}