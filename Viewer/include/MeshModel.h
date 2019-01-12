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
	std::vector<Vertex> boundingVer;
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
	
	GLuint boundingVAO;
	GLuint boundingVBO;

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

	bool wireOnlyMode;

	float KA;
	float KD;
	float KS;
	float sExp;

public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& textureCoords, const std::string& modelName = "");
	MeshModel(const MeshModel& ref, const std::string& name = "");
	virtual ~MeshModel();

	void xRotateObject(const float angle, bool inc = false);
	void yRotateObject(const float angle, bool inc = false);
	void zRotateObject(const float angle, bool inc = false);
	void translateObject(const float* translation, bool inc = false);
	void scaleObject(const float* scale, bool inc = false);
	void xRotateWorld(const float angle, bool inc = false);
	void yRotateWorld(const float angle, bool inc = false);
	void zRotateWorld(const float angle, bool inc = false);
	void translateWorld(const float* translation, bool inc = false);
	void scaleWorld(const float* scale, bool inc = false);

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
	void toggleWireFrameOnly();

	virtual void updateObjectTransform(const glm::mat4& mat);
	virtual void updateWorldTransform(const glm::mat4& mat);
};
