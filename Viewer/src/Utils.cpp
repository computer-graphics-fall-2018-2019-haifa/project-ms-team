#include "Utils.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
constexpr auto PI = 3.14159265359f;

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
	std::vector<glm::vec2> texture;
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
			texture.push_back(Utils::Vec2fFromStream(issLine));
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

	return MeshModel(faces, vertices, normals, texture, Utils::GetFileName(filePath));
}

glm::mat4 Utils::getRotationMatrix(const float angle, char axis)
{
	glm::mat4 mat(1);
	float deg = angle * PI / 180;
	float c = cos(deg);
	float s = sin(deg);
	if (axis == 'x') {
		mat[1][1] = c;
		mat[2][2] = c;
		mat[2][1] = -s;
		mat[1][2] = s;
	}
	else if (axis == 'y') {
		mat[0][0] = c;
		mat[2][2] = c;
		mat[2][0] = -s;
		mat[0][2] = s;
	}
	else if (axis == 'z') {
		mat[0][0] = c;
		mat[1][1] = c;
		mat[1][0] = -s;
		mat[0][1] = s;
	}
	return mat;
}

glm::mat4 Utils::getTranslationMatrix(const glm::vec3 vec)
{
	glm::mat4 mat(1);
	mat[3][0] = vec.x;
	mat[3][1] = vec.y;
	mat[3][2] = vec.z;
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

glm::vec4 Utils::pointProduct(const glm::vec4& v1, const glm::vec4& v2) {
	glm::vec4 r;
	r.x = v1.x * v2.x;
	r.y = v1.y * v2.y;
	r.z = v1.z * v2.z;
	r.w = v1.w * v2.w;
	return r;
}

char ** Utils::convertStringVectorToCharArray(std::vector<std::string> vec)
{
	int i = 0;
	char** strings = new char*[vec.size() + 1];
	for (auto s : vec) {
		strings[i] = new char[s.size() + 1];
		std::strcpy(strings[i++], s.c_str());
	}
	return strings;
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

