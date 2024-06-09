#version 430 core
layout(location = 0) in vec2 aTexcoord;

uniform mat4 proj_matrix;

out vec2 vs_texcoord;

void main() {
	gl_Position = proj_matrix * vec4(aTexcoord, 0, 1);
	vs_texcoord = aTexcoord;
}