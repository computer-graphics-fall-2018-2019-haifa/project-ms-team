#include "Light.h"

Light::Light(int type, std::string name) : MeshModel(std::vector<Face>(), std::vector<glm::vec3>(), std::vector<glm::vec3>(), name)
{
	this->type = type;
	this->direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	this->color = glm::vec4(1.0f);
	this->vertices.push_back(glm::vec3(0, 0, 0));
	this->intensity = glm::vec4(1.0f);
	this->pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

