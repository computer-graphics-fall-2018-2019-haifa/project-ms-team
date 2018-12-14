#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"
#include "Light.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Light>> lights;

	int activeCameraIndex;
	int activeModelIndex;
	int activeLightIndex;
	bool rainbowMode;
	bool circlesMode;

public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	const int GetModelCount() const;

	void AddCamera(const std::shared_ptr<Camera>& camera);
	const int GetCameraCount() const;

	void AddLight(const std::shared_ptr<Light>& light);
	const int GetLightCount() const;

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;

	void SetActiveLightIndex(int index);
	const int GetActiveLightIndex() const;

	const std::shared_ptr<MeshModel> getModel(int index) const;
	const std::shared_ptr<Camera> getCamera(int index) const;
	const std::shared_ptr<Light> getLight(int index) const;

	const std::vector<std::shared_ptr<Light>> getLights() const;

	void toggleRainbow();
	void toggleCircles();
	const bool getRainbow() const;
	const bool getCircles() const;
};