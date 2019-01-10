#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel, "Camera"),
	viewTransformation(1),
	worldViewTransformation(1),
	projectionTransformation(1)
{
	this->znear = 0.1f;
	this->zfar = 200.0f;
	this->aspect = 16.0f/9.0f;
	this->param1 = 10;
	this->SetCameraLookAt(eye, at, up);
	this->SetOrthographicProjection(param1, aspect, znear, zfar);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) {
	this->viewTransformation = glm::lookAt(eye, at, up);
	this->objectTransform = glm::inverse(this->viewTransformation);
	this->worldViewTransformation = glm::mat4(1);
	this->worldTransform = glm::mat4(1);
}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float zNear,
	const float zFar)
{
	float width = height * aspectRatio;
	this->zfar = zFar;
	this->znear = zNear;
	this->aspect = aspectRatio;
	this->param1 = height;
	this->projectionTransformation = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, zNear, zFar);
	this->activeView = 0;
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float zNear,
	const float zFar)
{
	this->zfar = zFar;
	this->znear = zNear;
	this->aspect = aspectRatio;
	this->param1 = fovy;
	this->projectionTransformation = glm::perspective(fovy, aspectRatio, zNear, zFar);
	this->activeView = 1;
}

void Camera::SetZoom(const float zoom) {
	float val = this->param1;
	if (this->activeView == 0) {
		this->SetOrthographicProjection(val, this->aspect, this->znear, this->zfar);
	}
	else if (this->activeView == 1) {
		val = glm::min(val, glm::pi<float>());
		this->SetPerspectiveProjection(val, this->aspect, this->znear, this->zfar);
	}
}

void Camera::SetAspectRatio(const float ratio)
{
	this->aspect = ratio;
}

glm::mat4x4 Camera::getViewTransformation()
{
	return this->viewTransformation;
}

glm::mat4x4 Camera::getWorldViewTransformation()
{
	return this->worldViewTransformation;
}

glm::mat4x4 Camera::getProjection()
{
	return this->projectionTransformation;
}

float Camera::getZNear() const
{
	return znear;
}

float Camera::getZFar() const
{
	return zfar;
}

float Camera::getParam() const
{
	return this->param1;
}

float Camera::getAspect() const
{
	return aspect;
}

bool Camera::isPerspective() const
{
	return activeView;
}
