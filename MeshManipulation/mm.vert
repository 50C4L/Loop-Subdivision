#version 400 

layout (location = 0) in vec3 inPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	mat4 model = mat4(1.0);

	gl_Position = projection * view * model * vec4(inPos, 1.0);
}