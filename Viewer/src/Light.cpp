#include "Light.h"

Light::Light(int type, std::string name) : MeshModel(std::vector<Face>(), std::vector<glm::vec3>(), std::vector<glm::vec3>(), name)
{
	this->type = type;
	this->direction = glm::vec3(1);
}

Light::~Light()
{
}

glm::vec3 Light::getDirection() const
{
	return this->direction;
}

int Light::getType() const
{
	return this->type;
}

void Light::setType(int type)
{
	this->type = type;
}

void Light::setDirection(glm::vec3 direction) {
	this->direction = direction;
}

