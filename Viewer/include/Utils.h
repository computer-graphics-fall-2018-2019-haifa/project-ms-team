#pragma once
#include <glm/glm.hpp>
#include <string>
#include "MeshModel.h"

/*
 * Utils class.
 * This class is consisted of static helper methods that can have many clients across the code.
 */
class Utils
{
public:
	static glm::vec3 Vec3fFromStream(std::istream& issLine);
	static glm::vec2 Vec2fFromStream(std::istream& issLine);
	static MeshModel LoadMeshModel(const std::string& filePath);
	static glm::mat4 getRotationMatrix(const float angle, char axis);
	static glm::mat4 getTranslationMatrix(const glm::vec3 vec);
	static glm::mat4 getScaleMatrix(const glm::vec3 vec);
	static char** convertStringVectorToCharArray(std::vector<std::string> vec);
	static glm::vec4 pointProduct(const glm::vec4& v1, const glm::vec4& v2);

	static std::vector<glm::vec3> CalculateNormals(std::vector<glm::vec3> vertices, std::vector<Face> faces);

private:
	static std::string GetFileName(const std::string& filePath);
};
