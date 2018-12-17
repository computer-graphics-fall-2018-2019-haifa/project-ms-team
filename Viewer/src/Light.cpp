#include "Light.h"

Light::Light(int type, std::string name) : MeshModel(std::vector<Face>(), std::vector<glm::vec3>(), std::vector<glm::vec3>(), name)
{
	this->type = type;
	this->direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	this->color = glm::vec4(1.0f);
	this->vertices.push_back(glm::vec3(0, 0, 0));
}

Light::~Light()
{
}

glm::vec3 Light::getDirection(const glm::mat4& mat) const
{
	glm::vec4 pos = mat * this->direction;
	glm::vec3 homPos(0.0f);
	homPos.x = pos.x / pos.w;
	homPos.y = pos.y / pos.w;
	homPos.z = pos.z / pos.w;
	return homPos;
}

int Light::getType() const
{
	return this->type;
}

void Light::setType(int type)
{
	this->type = type;
}

glm::vec3 Light::getLightPos(const glm::mat4 & mat) const
{
	glm::vec3 pos = this->getPosition();
	glm::vec4 homPos(pos.x, pos.y, pos.z, 1.0f);
	homPos = mat * (this->worldTransform * (this->objectTransform * homPos));
	pos.x = homPos.x / homPos.w;
	pos.y = homPos.y / homPos.w;
	pos.z = homPos.z / homPos.w;
	return pos;
}

void Light::setDirection(glm::vec3 direction) {
	this->direction.x = direction.x;
	this->direction.y = direction.y;
	this->direction.z = direction.z;
	this->direction.w = 1;
}

