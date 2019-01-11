#version 330 core

// Inputs from vertex shader (after interpolation was applied)
in vec4 fragPos;
in vec4 fragNormal;
in vec2 fragTexCoords;
in vec4 fragColor;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	frag_color = fragColor;
}
