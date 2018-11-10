#include "Scene.h"
#include "MeshModel.h"
#include <string>

Scene::Scene() :
	activeCameraIndex(-1),
	activeModelIndex(-1)
{

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

void Scene::SetActiveCameraIndex(int index)
{
	// implementation suggestion...
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
	// implementation suggestion...
	if (index >= 0 && index < models.size())
	{
		activeModelIndex = index;
	}
}

const int Scene::GetActiveModelIndex() const
{
	return activeModelIndex;
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
