#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"

/*
 * MeshModel class.
 * This class represents a mesh model (with faces and normals informations).
 * You can use Utils::LoadMeshModel to create instances of this class from .obj files.
 */
class MeshModel
{
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4x4 worldTransform;
	glm::mat4x4 objectTransform;
	glm::mat4x4 worldScaleTransform;
	glm::mat4x4 worldTranslationTransform;
	glm::mat4x4 worldxRotationTransform;
	glm::mat4x4 worldyRotationTransform;
	glm::mat4x4 worldzRotationTransform;
	glm::mat4x4 scaleTransform;	
	glm::mat4x4 translationTransform;
	glm::mat4x4 xRotationTransform;
	glm::mat4x4 yRotationTransform;
	glm::mat4x4 zRotationTransform;
	glm::vec4 color;
	std::string modelName;

public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	virtual ~MeshModel();

	void SetWorldTransformation(const glm::mat4x4& worldTransform);
	void SetObjectTransformation(const glm::mat4x4& objectTransform);
	void xRotateObject(const float angle);
	void yRotateObject(const float angle);
	void zRotateObject(const float angle);
	void translateObject(const float* translation);
	void scaleObject(const float* scale);
	void xRotateWorld(const float angle);
	void yRotateWorld(const float angle);
	void zRotateWorld(const float angle);
	void translateWorld(const float* translation);
	void scaleWorld(const float* scale);
	void updateWorldTransorm();
	void updateObjectTransorm();

	const glm::mat4x4& GetWorldTransformation() const;
	const glm::mat4x4& GetObjectTransformation() const;

	const glm::vec4& GetColor() const;
	void SetColor(const glm::vec4& color);

	const std::string& GetModelName();

	const std::vector<Face> getFaces() const;
	const std::vector<glm::vec3> getVertices() const;
	const std::vector<glm::vec3> getNormals() const;
	// Add more methods/functionality as needed...
};
