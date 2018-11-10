#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel.getFaces(), cameraModel.getVertices(), cameraModel.getNormals(), "Camera"),
	viewTransformation(1),
	zoom(1.0)
{
	this->SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) {
	glm::vec3 z = glm::normalize(eye - at);
	glm::vec3 x = glm::normalize(glm::cross(up, z));
	glm::vec3 y = glm::normalize(glm::cross(z, x));
	glm::mat4 mat(1);
	mat[0][0] = x.x; mat[0][1] = x.y; mat[0][2] = x.z;
	mat[1][0] = y.x; mat[1][1] = y.y; mat[1][2] = y.z;
	mat[2][0] = z.x; mat[2][1] = z.y; mat[2][2] = z.z;
	this->objectTransform = mat * Utils::getTranslationMatrix(-eye);
	this->worldTransform = glm::mat4(1);
	z = glm::normalize(at - eye);
	glm::normalize(glm::cross(z, up));
	glm::normalize(glm::cross(x, z));
	mat[0][0] = x.x; mat[0][1] = x.y; mat[0][2] = x.z;
	mat[1][0] = y.x; mat[1][1] = y.y; mat[1][2] = y.z;
	mat[2][0] = z.x; mat[2][1] = z.y; mat[2][2] = z.z;
	this->viewTransformation = mat * Utils::getTranslationMatrix(eye);
}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetZoom(const float zoom)
{

}

glm::mat4x4 Camera::getViewTransformation()
{
	return this->viewTransformation;
}

void Camera::xRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'x');
	this->xRotationTransform = m * this->xRotationTransform;
	updateObjectTransorm();
}

void Camera::yRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'y');
	this->yRotationTransform = m * this->yRotationTransform;
	updateObjectTransorm();
}

void Camera::zRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'z');
	this->zRotationTransform = m * this->zRotationTransform;
	updateObjectTransorm();
}

void Camera::translateObject(const float * translation)
{
	auto m = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	this->translationTransform = m * this->translationTransform;
	updateObjectTransorm();
}

void Camera::scaleObject(const float * scale)
{
	auto m = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	this->scaleTransform = m * this->scaleTransform;
	updateObjectTransorm();
}

void Camera::xRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'x');
	this->worldxRotationTransform = m * this->worldxRotationTransform;
	updateWorldTransorm();
}

void Camera::yRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'y');
	this->worldyRotationTransform = m * this->worldyRotationTransform;
	updateWorldTransorm();
}

void Camera::zRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'z');
	this->worldzRotationTransform = m * this->worldzRotationTransform;
	updateWorldTransorm();
}

void Camera::translateWorld(const float * translation)
{
	auto m = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	this->worldTranslationTransform = m * this->worldTranslationTransform;
	updateWorldTransorm();
}

void Camera::scaleWorld(const float * scale)
{
	auto m = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	this->worldScaleTransform = m * this->worldScaleTransform;
	updateWorldTransorm();
}

void Camera::updateObjectTransorm()
{
	this->objectTransform = translationTransform * zRotationTransform * yRotationTransform * xRotationTransform * scaleTransform;
}

void Camera::updateWorldTransorm()
{
	this->worldTransform = worldTranslationTransform * worldzRotationTransform * worldyRotationTransform * worldxRotationTransform * worldScaleTransform;
}
