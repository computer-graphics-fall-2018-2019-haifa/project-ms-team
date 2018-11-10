#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const MeshModel& cameraModel) :
	MeshModel(cameraModel.getFaces(), cameraModel.getVertices(), cameraModel.getNormals(), "Camera"),
	reverseTransformation(1),
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
	glm::vec3 y = glm::normalize(glm::cross(x, z));
	glm::mat4 mat(1);
	mat[0][0] = x.x; mat[0][1] = x.y; mat[0][2] = x.z;
	mat[1][0] = y.x; mat[1][1] = y.y; mat[1][2] = y.z;
	mat[2][0] = z.x; mat[2][1] = z.y; mat[2][2] = z.z;
	mat[3][0] = -eye.x; mat[3][1] = -eye.y; mat[3][2] = -eye.z;
	this->objectTransform = mat * Utils::getScaleMatrix(glm::vec3(10, 10, 10));
	glm::mat4 reverseMat(mat);
	reverseMat[3][0] = eye.x; reverseMat[3][1] = eye.y; reverseMat[3][2] = eye.z;
	this->reverseTransformation = reverseMat;
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

glm::mat4x4 Camera::getReverseTransformation()
{
	return this->reverseTransformation;
}
