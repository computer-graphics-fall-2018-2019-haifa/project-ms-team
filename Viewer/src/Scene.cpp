#include <iostream>
#include "Scene.h"
#include "MeshModel.h"
#include "Light.h"
#include <string>

Scene::Scene() :
	activeCameraIndex(-1),
	activeModelIndex(-1),
	activeLightIndex(-1),
	fogColor((0.5f, 0.5f, 0.5f, 1.0f)),
	ambientColor((0.8f, 0.8f, 0.8f, 1.0f))
{
	this->fogType = 0;
	this->shadingModel = 0;		// flat
	this->aliasLevel = 0;
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& model)
{
	models.push_back(model);
	activeModelIndex = (int)models.size() - 1;
}

const int Scene::GetModelCount() const
{
	return (int)models.size();
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras.push_back(camera);
	activeCameraIndex = (int)cameras.size() - 1;
}

const int Scene::GetCameraCount() const
{
	return (int)cameras.size();
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights.push_back(light);
	activeLightIndex = (int)lights.size() - 1;
}

const int Scene::GetLightCount() const
{
	return (int)lights.size();
}

void Scene::SetActiveCameraIndex(int index)
{
	if (index >= 0 && index < cameras.size())
	{
		activeCameraIndex = index;
	}
}

const int Scene::GetActiveCameraIndex() const
{
	return activeCameraIndex;
}

void Scene::SetActiveModelIndex(int index)
{
	if (index >= 0 && index < models.size())
	{
		activeModelIndex = index;
	}
}

const int Scene::GetActiveModelIndex() const
{
	return activeModelIndex;
}

void Scene::SetActiveLightIndex(int index)
{
	if (index >= 0 && index < lights.size())
	{
		activeLightIndex = index;
	}
}

const int Scene::GetActiveLightIndex() const
{
	return activeLightIndex;
}

const std::shared_ptr<MeshModel> Scene::getModel(int index) const
{
	if (index >= 0 && index < models.size()) {
		return this->models[index];
	}
	return nullptr;
}

const std::shared_ptr<Camera> Scene::getCamera(int index) const
{
	if (index >= 0 && index < cameras.size()) {
		return this->cameras[index];
	}
	return nullptr;
}

const std::shared_ptr<Light> Scene::getLight(int index) const
{
	if (index >= 0 && index < lights.size()) {
		return this->lights[index];
	}
	return nullptr;
}

const std::shared_ptr<MeshModel> Scene::getActiveModel() const
{
	return getModel(activeModelIndex);
}

const std::shared_ptr<Camera> Scene::getActiveCamera() const
{
	return getCamera(activeCameraIndex);
}

const std::shared_ptr<Light> Scene::getActiveLight() const
{
	return getLight(activeLightIndex);
}

const std::vector<std::shared_ptr<Light>> Scene::getLights() const
{
	return this->lights;
}

void Scene::applyFog(ShaderProgram& shader) const
{
	shader.setUniform("fog.fogType", fogType);
	shader.setUniform("fog.fogColor", fogColor);
	shader.setUniform("fog.density", fogDensity);
	shader.setUniform("fog.start", fogStart);
	shader.setUniform("fog.end", fogEnd);
}

int Scene::getShadingType() const
{
	return this->shadingModel;
}

void Scene::setShadingType(int type)
{
	this->shadingModel = type;
}

void Scene::setFogType(int type)
{
	this->fogType = type;
}

void Scene::setFogColor(const glm::vec4& color)
{
	this->fogColor = color;
}

void Scene::setFogBegin(float begin)
{
	this->fogStart = begin;
}

void Scene::setFogEnd(float end)
{
	this->fogEnd = end;
}

void Scene::setDensity(float den)
{
	this->fogDensity = den;
}

void Scene::setAmbientColor(const glm::vec4 & color)
{
	this->ambientColor = color;
}

const glm::vec4 & Scene::getAmbientColor() const
{
	return this->ambientColor;
}
