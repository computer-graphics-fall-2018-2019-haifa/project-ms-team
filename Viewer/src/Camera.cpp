#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel.getFaces(), cameraModel.getVertices(), cameraModel.getNormals(), "Camera"),
	viewTransformation(1),
	worldViewTransformation(1),
	projectionTransformation(1)
{
	this->znear = 0;
	this->zfar = 0;
	this->aspect = 1;
	this->param1 = 1;
	this->activeView = -1;
	this->SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) {
	glm::vec3 z = glm::normalize(eye - at);
	glm::vec3 x = glm::normalize(glm::cross(up, z));
	glm::vec3 y = glm::normalize(glm::cross(z, x));
	this->viewTransformation = glm::lookAt(eye, at, up);
	this->objectTransform = glm::inverse(this->viewTransformation);
	this->worldViewTransformation = glm::mat4(1);
	this->worldTransform = glm::mat4(1);
	this->worldScaleTransform = glm::mat4(1);
	this->worldTranslationTransform = glm::mat4(1);
	this->worldxRotationTransform = glm::mat4(1);
	this->worldzRotationTransform = glm::mat4(1);
	this->worldyRotationTransform = glm::mat4(1);
	this->scaleTransform = glm::mat4(1);
	this->translationTransform = glm::mat4(1);
	this->xRotationTransform = glm::mat4(1);
	this->yRotationTransform = glm::mat4(1);
	this->zRotationTransform = glm::mat4(1);
	this->pos = eye;
}

void Camera::xRotateObject(const float angle)
{
	MeshModel::xRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(-angle, 'x');
}

void Camera::yRotateObject(const float angle)
{
	MeshModel::yRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(-angle, 'y');
}

void Camera::zRotateObject(const float angle)
{
	MeshModel::zRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(-angle, 'z');
}

void Camera::translateObject(const float * translation)
{
	MeshModel::translateObject(translation);
	this->viewTransformation *= Utils::getTranslationMatrix(glm::vec3(-translation[0], -translation[1], -translation[2]));
}

void Camera::scaleObject(const float * scale)
{
	MeshModel::scaleObject(scale);
	this->viewTransformation *= Utils::getScaleMatrix(glm::vec3(1 / scale[0], 1 / scale[1], 1 / scale[2]));
}

void Camera::xRotateWorld(const float angle)
{
	MeshModel::xRotateWorld(angle);
	this->worldViewTransformation *= Utils::getRotationMatrix(-angle, 'x');
}

void Camera::yRotateWorld(const float angle)
{
	MeshModel::yRotateWorld(angle);
	this->worldViewTransformation *= Utils::getRotationMatrix(-angle, 'y');
}

void Camera::zRotateWorld(const float angle)
{
	MeshModel::zRotateWorld(angle);
	this->worldViewTransformation *= Utils::getRotationMatrix(-angle, 'z');
}

void Camera::translateWorld(const float * translation)
{
	MeshModel::translateWorld(translation);
	this->worldViewTransformation *= Utils::getTranslationMatrix(glm::vec3(-translation[0], -translation[1], -translation[2]));
}

void Camera::scaleWorld(const float * scale)
{
	MeshModel::scaleWorld(scale);
	this->worldViewTransformation *= Utils::getScaleMatrix(glm::vec3(1 / scale[0], 1 / scale[1], 1 / scale[2]));
}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{
	float width = height * aspectRatio;
	float right = near * width;
	float left = -right;
	float top = near * height;
	float bottom = -top;
	this->zfar = far;
	this->znear = near;
	this->aspect = aspectRatio;
	this->param1 = height;
	this->projectionTransformation = glm::ortho(left, right, bottom, top);
	this->activeView = 0;
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{
	this->zfar = far;
	this->znear = near;
	this->aspect = aspectRatio;
	this->param1 = fovy;
	this->projectionTransformation = glm::perspective(fovy, aspectRatio, near, far);
	this->activeView = 1;
}

void Camera::SetZoom(const float zoom) {
	if (this->activeView == 0) {
		this->SetOrthographicProjection(this->param1 * zoom, this->aspect, this->znear, this->zfar);
	}
	else if (this->activeView == 1) {
		this->SetPerspectiveProjection(this->param1 * zoom, this->aspect, this->znear, this->zfar);
	}
}

glm::mat4x4 Camera::getViewTransformation()
{
	return this->viewTransformation * Utils::getTranslationMatrix(pos);
}

glm::mat4x4 Camera::getWorldViewTransformation()
{
	return this->worldViewTransformation;
}

glm::mat4x4 Camera::getProjection()
{
	return this->projectionTransformation;
}
