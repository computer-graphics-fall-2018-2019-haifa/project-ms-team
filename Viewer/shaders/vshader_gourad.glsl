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

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// We set this field's properties from the C++ code
uniform Material material;
uniform vec4 SceneAmbient;
uniform vec4 lightPos[10];
uniform vec4 lightColor[10];

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// These outputs will be available in the fragment shader as inputs
out vec4 fragPos;
out vec4 fragNormal;
out vec2 fragTexCoords;
out vec4 fragColor;

void main()
{
	mat4 MVP = projection * view *  model;
	// Apply the model transformation to the 'position' and 'normal' properties of the vertex,
	// so the interpolated values of these properties will be available for us in the fragment shader

	vec4 pos = MVP * vec4(pos, 1.0f);
	gl_Position = pos;
	fragPos = pos;

	fragNormal = MVP * vec4(normal, 1.0f);

	// Pass the vertex texture coordinates property as is. Its interpolated value
	// will be avilable for us in the fragment shader
	fragTexCoords = texCoords;

	//lighting

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

	fragColor = clamp(IA + ID + IS, 0.0f, 1.0f);
}