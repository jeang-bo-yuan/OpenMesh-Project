#version 430 core
layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform MatrixBlock {
	mat4 view;
	mat4 proj;
} Matrix;
uniform mat4 Model;

flat out uint vsFaceID;

void main() {
	mat4 MVP = Matrix.proj * Matrix.view * Model;
	gl_Position = MVP * vec4(aPos, 1);

	vsFaceID = gl_VertexID / 3 + 1;
}