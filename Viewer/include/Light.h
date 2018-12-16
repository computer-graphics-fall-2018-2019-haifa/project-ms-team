#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "MeshModel.h"

class Light : public MeshModel
{
private:
	int type;
	glm::vec4 direction;
public:
	Light(int type, std::string name);
	~Light();
	glm::vec3 getDirection(const glm::mat4& mat) const;
	glm::vec3 getLightPos(const glm::mat4& mat) const;
	void setDirection(glm::vec3 direction);
	int getType() const;
	void setType(int type);
};

