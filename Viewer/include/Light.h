#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light
{
private:
	int type;
	glm::vec4 intensity;
	glm::vec4 pos;
	glm::vec3 direction;
public:
	Light(int type, std::string name);
	~Light();
	glm::vec4 getDirection() const;
	glm::vec4 getLightPos() const;
	void setDirection(glm::vec3 direction);
	void setXYZ(float* pos);
	int getType() const;
	void setType(int type);
	std::string getLightName();

	void setIntensity(const glm::vec4& i);
	const glm::vec4& getIntensity() const;
};

