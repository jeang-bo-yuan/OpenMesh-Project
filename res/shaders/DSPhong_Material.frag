// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 430
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

uniform MaterialInfo Material;
out vec4 vFragColor;

//lighting color
vec4 ambientColor  = vec4(0.1,0.1,0.1,1);
vec4 diffuseColor  = vec4(0.8,0.8,0.8,1);   
vec4 specularColor = vec4(1,1,1,1);
layout(location = 0) uniform sampler2DArray texture_array;
layout(std430, binding = 0) buffer selectedSSBO {
    int face_selected[];
};

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec3 vVaryingTexCoord;
flat in uint vFaceIndex;

float Shininess = 128.0;//for material specular

void main(void)
{ 
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));

    if (vVaryingTexCoord.z < 0)
        // Multiply intensity by diffuse color, force alpha to 1.0
        vFragColor = diff * diffuseColor * vec4(Material.Kd, 1);
    else
        vFragColor = diff * diffuseColor * texture(texture_array, vVaryingTexCoord);

    // Add in ambient light
    vFragColor += ambientColor;

    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) {
		spec = pow(spec, Shininess);
		vFragColor += specularColor * vec4(Material.Ka, 1) * spec;
    }

    if (face_selected[vFaceIndex] != 0) {
        // blend with red color, if the face is selected
        vFragColor = 0.5 * vFragColor +  vec4(0.5, 0, 0, 0.5);
    }
}
	
    