#include "Light.h"



Light::Light(int type, glm::vec3 pos, glm::vec3 direction)
{
	this->type = type;
	this->pos = pos;
	this->direction = direction;
}

Light::~Light()
{
}

glm::vec3 Light::getPos()
{
	return this->pos;
}

glm::vec3 Light::getDirection()
{
	return this->direction;
}

int Light::getType()
{
	return this->type;
}

void Light::setPos(glm::vec3 pos)
{
	this->pos = pos;
}

void Light::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}

void Light::setType(int type)
{
	this->type = type;
}
