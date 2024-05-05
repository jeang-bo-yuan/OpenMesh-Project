#version 430 core

uniform vec2 viewport;
// true -> select face this time
uniform bool selectFace;
// where to fetch ID
layout(binding = 0) uniform usampler2D IDtexture;

// Shader Storage Buffer, store selection result
layout(std430, binding = 0) buffer selectedSSBO {
    int face_selected[];
};


out vec4 FragColor;

void main() {
	if (selectFace) {
		vec2 texCoord = gl_FragCoord.xy / viewport.xy;

		uint ID = texture(IDtexture, texCoord).r;

		if (ID != 0) {
			face_selected[ID - 1] = 1;
		}
	}

	FragColor = vec4(0.12, 0.87, 0.88, 0.5);
}