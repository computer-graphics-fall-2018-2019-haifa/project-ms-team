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
	glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);

	auto lights = scene->getLights();

	glm::vec4 lightColors[10];
	glm::vec4 lightPos[10];

	// setup lights
	for (int i = 0; i < 10; ++i) {
		if (i < lights.size()) {
			lightColors[i] = lights[i]->getIntensity();
			if (lights[i]->getType()) {
				lightPos[i] = lights[i]->getLightPos();
			}
			else {
				lightPos[i] = lights[i]->getDirection();
			}
		}
		else {
			lightColors[i] = glm::vec4(0.0f);
			lightPos[i] = glm::vec4(0.0f);
		}
	}

	if (activeCamera != -1) {
		auto cam = scene->getCamera(activeCamera);
		viewMatrix = cam->getViewTransformation();
		worldViewMatrix = cam->getWorldViewTransformation();
		projection = cam->getProjection();
		cameraPos = cam->getPosition();
	}

	glm::mat4 view = viewMatrix * worldViewMatrix;

	colorShader.use();
	colorShader.setUniform("view", view);
	colorShader.setUniform("projection", projection);
	colorShader.setUniform("material.textureMap", 0);
	colorShader.setUniform("SceneAmbient", scene->getAmbientColor());
	colorShader.setUniform("lightColor", lightColors);
	colorShader.setUniform("lightPos", lightPos);
	

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
