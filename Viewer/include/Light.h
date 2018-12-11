#pragma once
#include <vector>
#include <glm/glm.hpp>

class Light
{
private:
	int type;
	glm::vec3 pos;
	glm::vec3 direction;
public:
	Light(int type, glm::vec3 pos = glm::vec3(0), glm::vec3 direction = glm::vec3(0));
	~Light();
	glm::vec3 getPos();
	glm::vec3 getDirection();
	int getType();
	void setPos(glm::vec3 pos);
	void setDirection(glm::vec3 direction);
	void setType(int type);
};

