#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light : public MeshModel
{
private:
	int type;
	float ld;
	glm::vec4 intensity;
	glm::vec4 pos;
	glm::vec3 direction;
public:
	Light(int type, std::string name);
	~Light();
	glm::vec3 getDirection() const;
	glm::vec4 getLightPos() const;
	void setDirection(glm::vec3 direction);
	void setXYZ(float* pos);
	int getType() const;
	void setType(int type);

	void setIntensity(const glm::vec4& i);
	const glm::vec4& getIntensity() const;
};

