#version 430 core

uniform vec2 regionStart;
uniform vec2 regionEnd;

void main() {
	if (gl_VertexID == 0) {
		gl_Position = vec4(regionStart, 0, 1);
	}
	else if (gl_VertexID == 1) {
		gl_Position = vec4(regionEnd.x, regionStart.y, 0, 1);
	}
	else if (gl_VertexID == 2) {
		gl_Position = vec4(regionEnd, 0, 1);
	}
	else if (gl_VertexID == 3) {
		gl_Position = vec4(regionStart.x, regionEnd.y, 0, 1);
	}
}