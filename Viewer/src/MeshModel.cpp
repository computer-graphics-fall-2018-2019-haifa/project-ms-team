#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1)),
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

void MeshModel::xRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'x');
	this->xRotationTransform = m * this->xRotationTransform;
	updateWorldTransorm();
}

void MeshModel::yRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'y');
	this->yRotationTransform = m * this->yRotationTransform;
	updateWorldTransorm();
}


void MeshModel::zRotateObject(const float angle)
{
	auto m = Utils::getRotationMatrix(angle, 'z');
	this->zRotationTransform = m * this->zRotationTransform;
	updateWorldTransorm();
}

void MeshModel::translateObject(const float * translation)
{
	auto m = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	this->translationTransform = m * this->translationTransform;
	updateWorldTransorm();
}

void MeshModel::scaleObject(const float * scale)
{
	auto m = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	this->scaleTransform = m * this->scaleTransform;
	updateWorldTransorm();
}

void MeshModel::updateWorldTransorm()
{
	this->worldTransform = translationTransform * zRotationTransform * yRotationTransform * xRotationTransform * scaleTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return this->worldTransform;
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
