#include "Light.h"

Light::Light(int type, std::string name)
{
	this->type = type;
	this->direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	this->intensity = glm::vec4(1.0f);
	this->pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

Light::~Light()
{
}

glm::vec4 Light::getDirection() const
{
	return glm::vec4(this->direction, 1.0f);
}

int Light::getType() const
{
	return this->type;
}

void Light::setType(int type)
{
	this->type = type;
}

std::string Light::getLightName()
{
	if (type) {
		return "Point";
	}
	return "Parallel";
}

void Light::setIntensity(const glm::vec4 & i)
{
	this->intensity = i;
}

const glm::vec4 & Light::getIntensity() const
{
	return this->intensity;
}

glm::vec4 Light::getLightPos() const
{
	return this->pos;
}

void Light::setDirection(glm::vec3 direction) {
	this->direction = direction;
}

void Light::setXYZ(float * pos)
{
	this->pos.x = pos[0];
	this->pos.y = pos[1];
	this->pos.z = pos[2];
}

