#version 330 core

struct Fog
{
	int fogType;
	vec4 fogColor;
	float density;
	float start;
	float end;
};

// Inputs from vertex shader (after interpolation was applied)
in vec4 fragPos;
in vec4 fragNormal;
in vec2 fragTexCoords;
in vec4 fragColor;

uniform Fog fog;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	frag_color = fragColor;

	// fog coloring
	float dist = abs(fragPos.z / fragPos.w);
	float fogFactor = 0.0f;
	// linear
	if (fog.fogType == 1) {
		fogFactor = clamp((fog.end-dist)/(fog.end-fog.start), 0.0f, 1.0f);
		frag_color = mix(fog.fogColor, frag_color, fogFactor);
	}
	else if (fog.fogType == 2) {	// exp
		fogFactor = clamp((1.0f/exp(dist * fog.density)), 0.0f, 1.0f);
		frag_color = mix(fog.fogColor, frag_color, fogFactor);
	}
	else if (fog.fogType == 3) {	//exp squared
		fogFactor = clamp(1.0 /exp((dist * fog.density) * (dist * fog.density)), 0.0f, 1.0f);
		frag_color = mix(fog.fogColor, frag_color, fogFactor);
	}
}
