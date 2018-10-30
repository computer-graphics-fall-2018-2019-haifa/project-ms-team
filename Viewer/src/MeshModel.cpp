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
	objectTransform(glm::mat4x4(1))
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
