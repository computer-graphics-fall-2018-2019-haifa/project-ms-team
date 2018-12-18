#include "Scene.h"
#include "MeshModel.h"
#include "Light.h"
#include <string>

Scene::Scene() :
	activeCameraIndex(-1),
	activeModelIndex(-1),
	activeLightIndex(-1),
	fogColor((0.8f, 0.8f, 0.8f, 1.0f))
{
	this->rainbowMode = false;
	this->circlesMode = false;
	this->aliasingMode = false;
	this->fogType = 0;
	this->shadingModel = 0;		// flat
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& model)
{
	models.push_back(model);
}

const int Scene::GetModelCount() const
{
	return (int)models.size();
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras.push_back(camera);
}

const int Scene::GetCameraCount() const
{
	return (int)cameras.size();
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights.push_back(light);
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

const std::vector<std::shared_ptr<Light>> Scene::getLights() const
{
	return this->lights;
}

void Scene::toggleRainbow()
{
	this->rainbowMode = !this->rainbowMode;
	this->circlesMode = this->circlesMode & !this->rainbowMode;
}

void Scene::toggleCircles()
{
	this->circlesMode = !this->circlesMode;
	this->rainbowMode = this->rainbowMode & !this->circlesMode;
}

void Scene::toggleAliasing()
{
	this->aliasingMode = !this->aliasingMode;
}

const bool Scene::getRainbow() const
{
	return this->rainbowMode;
}

const bool Scene::getCircles() const
{
	return this->circlesMode;
}

const bool Scene::getAliasing() const
{
	return this->aliasingMode;
}

void Scene::applyFog(glm::vec4& color, float z) const
{
	float f = 1.0f;
	switch (fogType) {
	case(0):
		break;
	case(1):
		f = (this->fogEnd - z) / (this->fogEnd - this->fogStart);
		break;
	case(2):
		f = std::exp(-z * this->fogDensity);
		break;
	case(3):
		f = std::exp(-std::pow(z * this->fogDensity, 2));
		break;
	}
	glm::clamp(f, 0.0f, 1.0f);
	color = glm::clamp(f * color + (1.0f - f) * this->fogColor, 0.0f, 1.0f);
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

void Scene::setFogColor(glm::vec4 color)
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
