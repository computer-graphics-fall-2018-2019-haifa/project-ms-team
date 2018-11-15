#pragma once
#include "Scene.h"
#include "Face.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

/*
 * Renderer class.
 */
class Renderer
{
private:
	float *colorBuffer;
	float *zBuffer;
	int viewportWidth;
	int viewportHeight;
	int viewportX;
	int viewportY;

	void putPixel(int x, int y, const glm::vec3& color);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();

	void drawModel(std::vector<Face> faces, std::vector<glm::vec3>, glm::vec4 color, bool rainbow = false);
	void drawNormals(std::vector<glm::vec3>, std::vector<Face> faces, std::vector<glm::vec3> normals, bool flip);
	
	std::vector<glm::vec3> applyTransfrom(std::vector<glm::vec3> ver, glm::mat4 mat);

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	void drawLine(float x1, float y1, float x2, float y2, glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	void drawBounding(std::vector<glm::vec3> vertices, glm::vec4 color);
	// Add more methods/functionality as needed...
};
