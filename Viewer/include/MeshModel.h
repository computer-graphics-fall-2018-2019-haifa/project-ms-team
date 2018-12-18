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
protected:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> boundingVer;
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
	bool drawBounding;
	bool drawNormals;
	bool drawFaceNormals;
	bool flipNormals;
	bool flipFaceNormals;
	float KA;
	float KD;
	float KS;
	float sExp;

public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	virtual ~MeshModel();

	virtual void xRotateObject(const float angle);
	virtual void yRotateObject(const float angle);
	virtual void zRotateObject(const float angle);
	virtual void translateObject(const float* translation);
	virtual void scaleObject(const float* scale);
	virtual void xRotateWorld(const float angle);
	virtual void yRotateWorld(const float angle);
	virtual void zRotateWorld(const float angle);
	virtual void translateWorld(const float* translation);
	virtual void scaleWorld(const float* scale);
	virtual void updateWorldTransorm();
	virtual void updateObjectTransorm();

	virtual const glm::mat4x4& GetWorldTransformation() const;
	virtual const glm::mat4x4& GetObjectTransformation() const;

	const glm::vec3& getPosition() const;
	const glm::vec4& GetColor() const;
	void SetColor(const glm::vec4& color);

	const std::string& GetModelName();

	const std::vector<Face> getFaces() const;
	const std::vector<glm::vec3> getVertices() const;
	const std::vector<glm::vec3> getNormals() const;
	const std::vector<glm::vec3> MeshModel::getBoundingVer() const;
	const bool isDrawBounding() const;
	const bool isDrawNormals() const;
	const bool isDrawFaceNormals() const;
	const bool isFlipNormals() const;
	const bool isFlipFaceNormals() const;

	const float getKAmbient() const;
	const float getKDiffuse() const;
	const float getKSpecular() const;
	const float getSpecularExp() const;

	void setKAmbient(float k);
	void setKDiffuse(float k);
	void setKSpecular(float k);
	void setSpecularExp(float k);


	void toggleBounding();
	void toggleNormals();
	void toggleFaceNormals();
	void toggleFlipNormals();
	void toggleFlipFaceNormals();
	// Add more methods/functionality as needed...
};
