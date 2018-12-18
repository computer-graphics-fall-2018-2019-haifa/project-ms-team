#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

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
	this->drawBounding = false;
	this->drawFaceNormals = false;
	this->drawNormals = false;
	this->flipNormals = false;
	this->flipFaceNormals = false;

	this->KA = 1.0f;
	this->KD = 1.0f;
	this->KS = 1.0f;
	this->sExp = 1.0f;


	glm::vec3 min((float)std::numeric_limits<int>::max());
	glm::vec3 max((float)std::numeric_limits<int>::min());

	for (auto ver : vertices) {
		if (ver.x < min.x)
			min.x = ver.x;
		if (ver.x > max.x)
			max.x = ver.x;
		if (ver.y < min.y)
			min.y = ver.y;
		if (ver.y > max.y)
			max.y = ver.y;
		if (ver.z < min.z)
			min.z = ver.z;
		if (ver.z > max.z)
			max.z = ver.z;
	}
	
	boundingVer.push_back(glm::vec3(min.x, min.y, min.z));
	boundingVer.push_back(glm::vec3(max.x, min.y, min.z));
	boundingVer.push_back(glm::vec3(min.x, max.y, min.z));
	boundingVer.push_back(glm::vec3(max.x, max.y, min.z));
	boundingVer.push_back(glm::vec3(min.x, min.y, max.z));
	boundingVer.push_back(glm::vec3(max.x, min.y, max.z));
	boundingVer.push_back(glm::vec3(min.x, max.y, max.z));
	boundingVer.push_back(glm::vec3(max.x, max.y, max.z));
}

MeshModel::~MeshModel()
{

}

void MeshModel::xRotateObject(const float angle)
{
	this->xRotationTransform = Utils::getRotationMatrix(angle, 'x');
	updateObjectTransorm();
}

void MeshModel::yRotateObject(const float angle)
{
	this->yRotationTransform = Utils::getRotationMatrix(angle, 'y');
	updateObjectTransorm();
}

void MeshModel::zRotateObject(const float angle)
{
	this->zRotationTransform = Utils::getRotationMatrix(angle, 'z');
	updateObjectTransorm();
}

void MeshModel::translateObject(const float * translation)
{
	this->translationTransform = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
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
	this->worldxRotationTransform = Utils::getRotationMatrix(angle, 'x');
	updateWorldTransorm();
}

void MeshModel::yRotateWorld(const float angle)
{
	this->worldyRotationTransform = Utils::getRotationMatrix(angle, 'y');
	updateWorldTransorm();
}

void MeshModel::zRotateWorld(const float angle)
{
	this->worldzRotationTransform = Utils::getRotationMatrix(angle, 'z');
	updateWorldTransorm();
}

void MeshModel::translateWorld(const float * translation)
{
	this->worldTranslationTransform = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
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

const glm::vec3 & MeshModel::getPosition() const
{
	return this->vertices[0];
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

const std::vector<glm::vec3> MeshModel::getBoundingVer() const
{
	return this->boundingVer;
}

const bool MeshModel::isDrawBounding() const
{
	return this->drawBounding;
}

const bool MeshModel::isDrawNormals() const
{
	return this->drawNormals;
}

const bool MeshModel::isFlipNormals() const
{
	return this->flipNormals;
}

const bool MeshModel::isFlipFaceNormals() const
{
	return this->flipFaceNormals;
}

const float MeshModel::getKAmbient() const
{
	return this->KA;
}

const float MeshModel::getKDiffuse() const
{
	return KD;
}

const float MeshModel::getKSpecular() const
{
	return this->KS;
}

const float MeshModel::getSpecularExp() const
{
	return this->sExp;
}

const bool MeshModel::isDrawFaceNormals() const
{
	return this->drawFaceNormals;
}

void MeshModel::setKAmbient(float k)
{
	this->KA = k;
}

void MeshModel::setKDiffuse(float k)
{
	this->KD = k;
}

void MeshModel::setKSpecular(float k)
{
	this->KS = k;
}

void MeshModel::setSpecularExp(float k)
{
	this->sExp = k;
}

void MeshModel::toggleBounding()
{
	this->drawBounding = !this->drawBounding;
}

void MeshModel::toggleNormals()
{
	this->drawNormals = !this->drawNormals;
}

void MeshModel::toggleFaceNormals()
{
	this->drawFaceNormals = !this->drawFaceNormals;
}

void MeshModel::toggleFlipNormals()
{
	this->flipNormals = !this->flipNormals;
}

void MeshModel::toggleFlipFaceNormals()
{
	this->flipFaceNormals = !this->flipFaceNormals;
}

const std::vector<glm::vec3> MeshModel::getNormals() const
{
	return this->normals;
}
