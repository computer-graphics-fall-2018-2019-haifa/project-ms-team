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
	// Add here more static utility functions...
	// For example:
	//	anything else that may be useful in global scope

private:
	static std::string GetFileName(const std::string& filePath);
};
