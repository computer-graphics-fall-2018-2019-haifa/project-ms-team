#include "Light.h"

Light::Light(int type, std::string name) : MeshModel(std::vector<Face>(), std::vector<glm::vec3>(), std::vector<glm::vec3>(), name)
{
	this->type = type;
	this->direction = glm::vec3(0);
}

Light::~Light()
{
}

glm::vec3 Light::getDirection()
{
	return this->direction;
}

int Light::getType()
{
	return this->type;
}

void Light::setType(int type)
{
	this->type = type;
}

