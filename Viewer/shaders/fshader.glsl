#version 330 core

struct Material
{
	sampler2D textureMap;

	vec4 AmbientColor;
	vec4 DiffuseColor;
	vec4 SpecualrColor;

	float KA;
	float KD;
	float KS;
	float KSE;
};

// We set this field's properties from the C++ code
uniform Material material;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));

	frag_color = material.AmbientColor;
}
