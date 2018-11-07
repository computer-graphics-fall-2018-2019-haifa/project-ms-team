#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
	worldTransform(glm::mat4x4(1)),
	objectTransform(glm::mat4x4(1)),
	worldScaleTransform(glm::mat4x4(1)),
	worldTranslationTransform(glm::mat4x4(1)),
	worldxRotationTransform(glm::mat4x4(1)),
	worldyRotationTransform(glm::mat4x4(1)),
	worldzRotationTransform(glm::mat4x4(1)),
	scaleTransform(glm::mat4x4(1)),
	translationTransform(glm::mat4x4(1)),
	xRotationTransform(glm::mat4x4(1)),
	yRotationTransform(glm::mat4x4(1)),
	zRotationTransform(glm::mat4x4(1))
{
	this->faces = faces;
	this->vertices = vertices;
	this->normals = normals;
}

MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

void MeshModel::SetObjectTransformation(const glm::mat4x4& objectTransform)
{
	this->objectTransform = objectTransform;
}

void MeshModel::xRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'x');
	this->xRotationTransform = m * this->xRotationTransform;
	updateObjectTransorm();
}

void MeshModel::yRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'y');
	this->yRotationTransform = m * this->yRotationTransform;
	updateObjectTransorm();
}

void MeshModel::zRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'z');
	this->zRotationTransform = m * this->zRotationTransform;
	updateObjectTransorm();
}

void MeshModel::translateObject(const float * translation)
{
	auto m = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	this->translationTransform = m * this->translationTransform;
	updateObjectTransorm();
}

void MeshModel::scaleObject(const float * scale)
{
	auto m = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	this->scaleTransform = m * this->scaleTransform;
	updateObjectTransorm();
}

void MeshModel::xRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'x');
	this->worldxRotationTransform = m * this->worldxRotationTransform;
	updateWorldTransorm();
}

void MeshModel::yRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'y');
	this->worldyRotationTransform = m * this->worldyRotationTransform;
	updateWorldTransorm();
}

void MeshModel::zRotateWorld(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'z');
	this->worldzRotationTransform = m * this->worldzRotationTransform;
	updateWorldTransorm();
}

void MeshModel::translateWorld(const float * translation)
{
	auto m = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	this->worldTranslationTransform = m * this->worldTranslationTransform;
	updateWorldTransorm();
}

void MeshModel::scaleWorld(const float * scale)
{
	auto m = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	this->worldScaleTransform = m * this->worldScaleTransform;
	updateWorldTransorm();
}

void MeshModel::updateWorldTransorm()
{
	this->worldTransform = worldTranslationTransform * worldzRotationTransform * worldyRotationTransform * worldxRotationTransform * worldScaleTransform;
}

void MeshModel::updateObjectTransorm()
{
	this->objectTransform = translationTransform * zRotationTransform * yRotationTransform * xRotationTransform * scaleTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return this->worldTransform;
}

const glm::mat4x4 & MeshModel::GetObjectTransformation() const
{
	return this->objectTransform;
}

void MeshModel::SetColor(const glm::vec4& color)
{
	this->color = color;
}

const glm::vec4& MeshModel::GetColor() const
{
	return color;
}

const std::string& MeshModel::GetModelName()
{
	return modelName;
}

const std::vector<Face> MeshModel::getFaces() const
{
	return this->faces;
}

const std::vector<glm::vec3> MeshModel::getVertices() const
{
	return this->vertices;
}

const std::vector<glm::vec3> MeshModel::getNormals() const
{
	return this->normals;
}
