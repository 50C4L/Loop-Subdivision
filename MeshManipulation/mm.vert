#version 400 

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 normal;
out vec3 fragPos;

void main()
{
	fragPos = vec3( model * vec4(inPos,1.0) );
	normal = inNorm;
	gl_Position = projection * view * model * vec4(inPos, 1.0);
}