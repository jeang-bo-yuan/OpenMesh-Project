#version 430 core

// don't interpolate
flat in uint vsFaceID;

out uvec4 FragColor;

void main() {
	FragColor.r = vsFaceID;
}