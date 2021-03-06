#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"
#include "Light.h"
#include "ShaderProgram.h"

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
	int fogType;
	glm::vec4 fogColor;
	glm::vec4 ambientColor;

	int shadingModel;

	float fogEnd;
	float fogStart;
	float fogDensity;
	int aliasLevel;

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

	const std::shared_ptr<MeshModel> getActiveModel() const;
	const std::shared_ptr<Camera> getActiveCamera() const;
	const std::shared_ptr<Light> getActiveLight() const;


	const std::vector<std::shared_ptr<Light>> getLights() const;

	void applyFog(ShaderProgram& shader) const;
	
	int getShadingType() const;
	void setShadingType(int type);

	void setFogType(int type);
	void setFogColor(const glm::vec4& color);

	void setFogBegin(float begin);
	void setFogEnd(float end);
	void setDensity(float den);

	void setAmbientColor(const glm::vec4& color);
	const glm::vec4& getAmbientColor() const;
};