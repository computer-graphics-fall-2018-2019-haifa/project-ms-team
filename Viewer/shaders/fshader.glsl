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

struct Fog
{
	int fogType;
	vec4 fogColor;
	float density;
	float start;
	float end;
};

// We set this field's properties from the C++ code
uniform Material material;
uniform Fog fog;
uniform vec4 SceneAmbient;
uniform vec4 lightPos[10];
uniform vec4 lightColor[10];

// Inputs from vertex shader (after interpolation was applied)
in vec4 fragPos;
in vec4 fragNormal;
in vec2 fragTexCoords;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	vec4 N = normalize(fragNormal);
	vec4 V = normalize(fragPos);	//assuming camera is always at 0,0,0
	// ambient is only needed once
	vec4 ac = material.KA * material.AmbientColor;
	vec4 IA = clamp(vec4(ac.x * SceneAmbient.x, ac.y * SceneAmbient.y, ac.z * SceneAmbient.z, 1.0f), 0.0f, 1.0f);

	vec4 ID = vec4(0.0f);
	vec4 IS = vec4(0.0f);

	for (int i=0; i<10; i++) {
		vec4 lightColor = lightColor[i];
		vec4 pos = lightPos[i];

		vec4 L = normalize(pos - fragPos);
		vec4 R = normalize(reflect(-L, N));

		float LN = max(dot(N, L), 0.0f);
		vec4 dc = material.KD * LN * material.DiffuseColor;
		ID = ID + clamp(vec4(dc.x * lightColor.x, dc.y * lightColor.y, dc.z * lightColor.z, 1.0f), 0.0f, 1.0f);

		float RV = pow(max(dot(R, V), 0.0f), material.KSE);
		vec4 sc = material.KS * RV * material.SpecualrColor;
		IS = IS + clamp(vec4(sc.x * lightColor.x, sc.y * lightColor.y, sc.z * lightColor.z, 1.0f), 0.0f, 1.0f);
	}
	frag_color = clamp(IA + ID + IS, 0.0f, 1.0f);
	
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
