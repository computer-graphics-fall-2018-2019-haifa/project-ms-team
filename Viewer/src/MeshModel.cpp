#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& textureCoords, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1.0f)),
	objectTransform(glm::mat4x4(1.0f)),
	worldScaleTransform(glm::mat4x4(1.0f)),
	worldTranslationTransform(glm::mat4x4(1.0f)),
	worldxRotationTransform(glm::mat4x4(1.0f)),
	worldyRotationTransform(glm::mat4x4(1.0f)),
	worldzRotationTransform(glm::mat4x4(1.0f)),
	scaleTransform(glm::mat4x4(1.0f)),
	translationTransform(glm::mat4x4(1.0f)),
	xRotationTransform(glm::mat4x4(1.0f)),
	yRotationTransform(glm::mat4x4(1.0f)),
	zRotationTransform(glm::mat4x4(1.0f))
{
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
	// add color and stuff later
	modelVertices.reserve(3 * faces.size());
	for (auto face : faces) {
		for (int i = 0; i < 3; ++i) {
			Vertex vertex;
			vertex.position = vertices[face.GetVertexIndex(i)];
			vertex.normal = normals[face.GetNormalIndex(i)];
			if (textureCoords.size() > 0) {
				vertex.tex = textureCoords[face.GetTextureIndex(i)];
			}
			else {
				vertex.tex = glm::vec2(0.0f, 0.0f);
			}
			modelVertices.push_back(vertex);
		}
	}
	//GL stuff
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// load vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	
	// load normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	
	// load textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));

	// unbind
	glBindVertexArray(0);
}

MeshModel::MeshModel(const MeshModel & ref, const std::string & name)
{
	this->drawBounding = false;
	this->drawFaceNormals = false;
	this->drawNormals = false;
	this->flipNormals = false;
	this->flipFaceNormals = false;

	this->KA = 1.0f;
	this->KD = 1.0f;
	this->KS = 1.0f;
	this->sExp = 1.0f;

	this->boundingVer = ref.boundingVer;
	this->modelVertices = ref.modelVertices;

	this->modelName = ref.modelName;
	this->objectTransform = ref.objectTransform;
	this->worldTransform = ref.worldTransform;
	
	this->worldScaleTransform = ref.worldScaleTransform;
	this->worldTranslationTransform = ref.worldTranslationTransform;
	this->worldxRotationTransform = ref.worldxRotationTransform;
	this->worldyRotationTransform = ref.worldyRotationTransform;
	this->worldzRotationTransform = ref.worldzRotationTransform;
	
	this->scaleTransform = ref.worldTransform;
	this->translationTransform = ref.translationTransform;
	this->xRotationTransform = ref.xRotationTransform;
	this->yRotationTransform = ref.yRotationTransform;
	this->zRotationTransform = ref.zRotationTransform;

	//GL stuff
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// load vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

	// load normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// load textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));

	// unbind
	glBindVertexArray(0);
}

MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void MeshModel::drawModel(ShaderProgram& shader, Texture2D& tex) const
{
	shader.setUniform("model", worldTransform * objectTransform);
	shader.setUniform("material.AmbientColor", colorAmbient);
	shader.setUniform("material.DiffuseColor", colorDiffuse);
	shader.setUniform("material.SpecualrColor", colorSpecular);
	shader.setUniform("material.KA", KA);
	shader.setUniform("material.KD", KD);
	shader.setUniform("material.KS", KS);
	shader.setUniform("material.KSE", sExp);

	tex.bind(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)modelVertices.size());
	glBindVertexArray(0);
	tex.unbind(0);

	shader.setUniform("material.AmbientColor", colorLine);
	shader.setUniform("material.DiffuseColor", colorLine);
	shader.setUniform("material.SpecualrColor", colorLine);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)modelVertices.size());
	glBindVertexArray(0);
}

void MeshModel::xRotateObject(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'x');;
	xRotationTransform = mat;
	updateObjectTransform(mat);
}

void MeshModel::yRotateObject(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'y');
	yRotationTransform = mat;
	updateObjectTransform(mat);
}

void MeshModel::zRotateObject(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'z');
	zRotationTransform = mat;
	updateObjectTransform(mat);
}

void MeshModel::translateObject(const float * translation)
{
	const glm::mat4 mat = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	translationTransform = mat;
	updateObjectTransform(mat);
}

void MeshModel::scaleObject(const float * scale)
{
	const glm::mat4 mat = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	scaleTransform = mat;
	updateObjectTransform(mat);
}

void MeshModel::xRotateWorld(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'x');
	worldxRotationTransform = mat;
	updateWorldTransform(mat);
}

void MeshModel::yRotateWorld(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'y');
	worldyRotationTransform = mat;
	updateWorldTransform(mat);
}

void MeshModel::zRotateWorld(const float angle)
{
	const glm::mat4 mat = Utils::getRotationMatrix(angle, 'z');
	worldzRotationTransform = mat;
	updateWorldTransform(mat);
}

void MeshModel::translateWorld(const float * translation)
{
	const glm::mat4 mat = Utils::getTranslationMatrix(glm::vec3(translation[0], translation[1], translation[2]));
	worldTranslationTransform = mat;
	updateWorldTransform(mat);
}

void MeshModel::scaleWorld(const float * scale)
{
	const glm::mat4 mat = Utils::getScaleMatrix(glm::vec3(scale[0], scale[1], scale[2]));
	worldScaleTransform = mat;
	updateWorldTransform(mat);
}

const glm::vec3 & MeshModel::getPosition() const
{
	glm::vec4 temp(0.0f, 0.0f, 0.0f, 1.0f);
	temp = worldTransform * objectTransform * temp;
	glm::vec3 t(temp.x, temp.y, temp.z);
	return t / temp.w;
}

const std::string& MeshModel::GetModelName()
{
	return modelName;
}

void MeshModel::SetAmbientColor(const glm::vec4 & color)
{
	this->colorAmbient = color;
}

void MeshModel::SetDiffuseColor(const glm::vec4 & color)
{
	this->colorDiffuse = color;
}

void MeshModel::SetSpecularColor(const glm::vec4 & color)
{
	this->colorSpecular = color;
}

void MeshModel::SetLineColor(const glm::vec4 & color)
{
	this->colorLine = color;
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

void MeshModel::updateObjectTransform(const glm::mat4 & mat)
{
	objectTransform = translationTransform * xRotationTransform * yRotationTransform * zRotationTransform * scaleTransform;
}

void MeshModel::updateWorldTransform(const glm::mat4 & mat)
{
	worldTransform = worldTranslationTransform * worldxRotationTransform * worldyRotationTransform * worldzRotationTransform * worldScaleTransform;
}
