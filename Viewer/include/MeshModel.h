#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex;
};
class MeshModel
{
protected:
	std::vector<glm::vec3> boundingVer;
	std::vector<Vertex> modelVertices;

	std::string modelName;

	glm::mat4x4 worldTransform;
	glm::mat4x4 objectTransform;

	glm::vec4 colorAmbient;
	glm::vec4 colorDiffuse;
	glm::vec4 colorSpecular;
	glm::vec4 colorLine;

	GLuint vao;
	GLuint vbo;
	
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
	
	
	bool drawBounding;
	bool drawNormals;
	bool drawFaceNormals;
	bool flipNormals;
	bool flipFaceNormals;

	bool textureAvailable;

	float KA;
	float KD;
	float KS;
	float sExp;

public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& textureCoords, const std::string& modelName = "");
	MeshModel(const MeshModel& ref, const std::string& name = "");
	virtual ~MeshModel();

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

	const std::string& GetModelName();

	void SetAmbientColor(const glm::vec4& color);
	void SetDiffuseColor(const glm::vec4& color);
	void SetSpecularColor(const glm::vec4& color);
	void SetLineColor(const glm::vec4& color);
	void setKAmbient(float k);
	void setKDiffuse(float k);
	void setKSpecular(float k);
	void setSpecularExp(float k);

	void drawModel(ShaderProgram& shader, Texture2D& tex) const;

	const glm::vec3 getPosition() const;

	void toggleBounding();
	void toggleNormals();
	void toggleFaceNormals();
	void toggleFlipNormals();
	void toggleFlipFaceNormals();

	void updateObjectTransform(const glm::mat4& mat);
	void updateWorldTransform(const glm::mat4& mat);
};
