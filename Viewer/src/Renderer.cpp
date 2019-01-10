#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Render(std::shared_ptr<Scene>  scene) {
	int activeCamera = scene->GetActiveCameraIndex();
	glm::mat4 viewMatrix(1.0f);
	glm::mat4 worldViewMatrix(1.0f);
	glm::mat4 projection(1.0f);
	glm::vec3 cameraPos(0.0f, 0.0f, 1.0f);

	auto lights = scene->getLights();

	if (activeCamera != -1) {
		auto cam = scene->getCamera(activeCamera);
		viewMatrix = cam->getViewTransformation();
		worldViewMatrix = cam->getWorldViewTransformation();
		projection = cam->getProjection();
	}

	glm::mat4 view = viewMatrix * worldViewMatrix;

	colorShader.use();
	colorShader.setUniform("view", view);
	colorShader.setUniform("projection", projection);
	colorShader.setUniform("material.textureMap", 0);

	for (int i = 0; i < scene->GetModelCount(); ++i) {
		auto model = scene->getModel(i);
		model->drawModel(colorShader, texture1);
	}
}

void Renderer::LoadShaders()
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
}

void Renderer::LoadTextures()
{
	if (!texture1.loadTexture("bin\\Debug\\crate.jpg", true))
	{
		texture1.loadTexture("bin\\Release\\crate.jpg", true);
	}
}
