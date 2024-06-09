#version 430 core

layout(location=0) uniform sampler2DArray texture_array;
uniform int layer;
uniform bool is_wireframe;

in vec2 vs_texcoord;

out vec4 FragColor;

void main() {
	if (is_wireframe || layer == -1)
		FragColor = vec4(0, 0, 0, 1);
	else
		FragColor = texture(texture_array, vec3(vs_texcoord, layer));
}