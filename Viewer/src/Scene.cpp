#include "Scene.h"
#include "MeshModel.h"
#include "Light.h"
#include <string>

Scene::Scene() :
	activeCameraIndex(-1),
	activeModelIndex(-1),
	activeLightIndex(-1)
{
	this->rainbowMode = false;
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
}

const bool Scene::getRainbow() const
{
	return this->rainbowMode;
}
