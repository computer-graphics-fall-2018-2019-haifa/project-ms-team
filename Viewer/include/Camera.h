#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"
#include "Utils.h"

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 *
 * Implementation suggestion:
 * --------------------------
 * Make the Camera class be a subclass of MeshModel, so you can easily and elegantly render 
 * the cameras you have added to the scene.
 */
class Camera : public MeshModel {
private:
	glm::mat4x4 viewTransformation;
	glm::mat4x4 worldViewTransformation;
	glm::mat4x4 projectionTransformation;
	float znear;
	float zfar;
	float aspect;
	float param1;
	int activeView;

public:
	Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel);
	~Camera();

	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);

	void SetOrthographicProjection(
		const float height,
		const float aspectRatio,
		const float near,
		const float far);

	void SetPerspectiveProjection(
		const float fovy,
		const float aspect,
		const float near,
		const float far);

	void SetZoom(const float zoom);

	glm::mat4x4 getViewTransformation();
	glm::mat4x4 getWorldViewTransformation();
	glm::mat4x4 getProjection();

	virtual void updateWorldTransorm();
	virtual void updateObjectTransorm();
};
