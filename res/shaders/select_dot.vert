#version 430 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform MatrixBlock {
	mat4 view;
	mat4 proj;
} Matrix;

void main() {
	vec4 view_pos = Matrix.view * vec4(aPos, 1);
	gl_Position = Matrix.proj * view_pos;
	float d = distance(vec3(0, 0, 0), view_pos.xyz);
	gl_PointSize = max(0.5 / d, 0);
}