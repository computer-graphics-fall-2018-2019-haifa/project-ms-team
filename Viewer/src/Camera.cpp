#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel.getFaces(), cameraModel.getVertices(), cameraModel.getNormals(), "Camera"),
	viewTransformation(1),
	worldViewTransformation(1),
	projectionTransformation(1),
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
	mat[0][0] = x.x; mat[0][1] = y.x; mat[0][2] = z.x; mat[3][0] = glm::dot(x, eye);
	mat[1][0] = x.y; mat[1][1] = y.y; mat[1][2] = z.y; mat[3][1] = glm::dot(y, eye);
	mat[2][0] = x.z; mat[2][1] = y.z; mat[2][2] = z.z; mat[3][2] = glm::dot(z, eye);
	this->viewTransformation = mat;
	this->objectTransform = glm::inverse(mat);
	// resetting all the transformations
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
	glm::mat4 mat(1);
	mat[0][0] = 2 / (right - left);
	mat[1][1] = 2 / (top - bottom);
	mat[2][2] = 2 / (near - far);
	mat[3][0] = -(right + left) / (right - left);
	mat[3][1] = -(top + bottom) / (top - bottom);
	mat[3][2] = -(far + near) / (far - near);
	this->projectionTransformation = mat;
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{
	float height = glm::tan(fovy /2);
	float width = height * aspectRatio;
	float right = width * near;
	float left = -right;
	float top = height * near;
	float bottom = -top;
	glm::mat4 mat(0);
	mat[0][0] = (2 * near) / width;
	mat[1][1] = -(2 * near) / height;
	mat[2][2] = -(far + near) / (far - near);
	mat[3][2] = -(2 * far * near) / (far - near);
	mat[2][3] = -1;
	mat[2][0] = (right + left) / (right - left);
	mat[2][1] = (top + bottom) / (top - bottom);
	this->projectionTransformation = mat;
}

void Camera::SetZoom(const float zoom) {
	float z[3] = { zoom, zoom, zoom };
	this->scaleObject(z);
}

glm::mat4x4 Camera::getViewTransformation()
{
	return this->worldViewTransformation * this->viewTransformation;
}

glm::mat4x4 Camera::getWorldViewTransformation()
{
	return this->worldViewTransformation;
}

glm::mat4x4 Camera::getProjection()
{
	return this->projectionTransformation;
}
