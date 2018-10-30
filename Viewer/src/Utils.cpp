#include "Utils.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

glm::vec3 Utils::Vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 Utils::Vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

MeshModel Utils::LoadMeshModel(const std::string& filePath)
{
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::ifstream ifile(filePath.c_str());

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		std::string curLine;
		std::getline(ifile, curLine);

		// read the type of the line
		std::istringstream issLine(curLine);
		std::string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v") {
			vertices.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vn") {
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt") {
			// Texture coordinates
		}
		else if (lineType == "f") {
			faces.push_back(Face(issLine));
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else {
			std::cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}

	return MeshModel(faces, vertices, normals, Utils::GetFileName(filePath));
}

glm::mat4 Utils::getRotationMatrix(const float angle, char axis)
{
	glm::mat4 mat(1);
	float c = cos(angle);
	float s = sin(angle);
	if (axis == 'x') {
		mat[1][1] = c;
		mat[2][2] = c;
		mat[2][3] = -s;
		mat[3][2] = s;
	}
	else if (axis == 'y') {
		mat[0][0] = c;
		mat[2][2] = c;
		mat[1][3] = -s;
		mat[3][1] = s;
	}
	else if (axis == 'z') {
		mat[0][0] = c;
		mat[1][1] = c;
		mat[0][1] = -s;
		mat[1][0] = s;
	}
	return mat;
}

glm::mat4 Utils::getTranslationMatrix(const glm::vec3 vec)
{
	glm::mat4 mat(1);
	mat[0][3] = vec.x;
	mat[1][3] = vec.y;
	mat[2][3] = vec.z;
	return mat;
}

glm::mat4 Utils::getScaleMatrix(const glm::vec3 vec)
{
	glm::mat4 mat(1);
	mat[0][0] = vec.x;
	mat[1][1] = vec.y;
	mat[2][2] = vec.z;
	return mat;
}

std::string Utils::GetFileName(const std::string& filePath)
{
	if (filePath.empty()) {
		return {};
	}

	auto len = filePath.length();
	auto index = filePath.find_last_of("/\\");

	if (index == std::string::npos) {
		return filePath;
	}

	if (index + 1 >= len) {

		len--;
		index = filePath.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filePath;
		}

		if (index == 0) {
			return filePath.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filePath.substr(0, len);
		}

		return filePath.substr(index + 1, len - index - 1);
	}

	return filePath.substr(index + 1, len - index);
}