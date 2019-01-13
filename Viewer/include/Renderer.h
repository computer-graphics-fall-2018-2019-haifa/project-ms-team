#pragma once
#include "Scene.h"
#include "Face.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "ShaderProgram.h"
#include "Texture2D.h"

class Renderer
{
private:
	ShaderProgram lightShader;
	ShaderProgram colorShader;
public:
	Renderer();
	~Renderer();

	void Render(std::shared_ptr<Scene> scene);
	void LoadPhongShaders();
	void LoadGouradShaders();

	void setNumColors(int num);
};
