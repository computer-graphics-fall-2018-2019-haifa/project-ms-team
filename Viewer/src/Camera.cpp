#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel.getFaces(), cameraModel.getVertices(), cameraModel.getNormals(), "Camera"),
	viewTransformation(1),
	worldViewTransformation(1),
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
	this->translationTransform = mat * Utils::getTranslationMatrix(-eye);
	updateObjectTransorm();
	this->worldTransform = glm::mat4(1);
	this->viewTransformation = glm::inverse(this->objectTransform);
}

void Camera::xRotateObject(const float angle)
{
	MeshModel::xRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(angle, 'x');
}

void Camera::yRotateObject(const float angle)
{
	MeshModel::yRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(angle, 'y');
}

void Camera::zRotateObject(const float angle)
{
	MeshModel::zRotateObject(angle);
	this->viewTransformation *= Utils::getRotationMatrix(angle, 'z');
}

void Camera::translateObject(const float * translation)
{
	MeshModel::translateObject(translation);
	this->viewTransformation *= Utils::getTranslationMatrix(glm::vec3(-translation[0], -translation[1], -translation[2]));
}

void Camera::scaleObject(const float * scale)
{
	MeshModel::scaleObject(scale);
	this->viewTransformation *= Utils::getScaleMatrix(glm::vec3(1 / scale[0], 1 / scale[1], 1 / scale[2]));;
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
	this->worldViewTransformation *= Utils::getScaleMatrix(glm::vec3(1 / scale[0], 1 / scale[1], 1 / scale[2]));;
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
	return this->worldViewTransformation * this->viewTransformation;
}
