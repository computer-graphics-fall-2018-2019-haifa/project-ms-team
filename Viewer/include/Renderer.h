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
	bool aliasing;

	void putPixel(int x, int y, float z, const glm::vec3& color);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();
	
	void drawNormals(const std::vector<glm::vec3>&, const std::vector<Face>& faces, const std::vector<glm::vec3>& normals, bool flip);
	void drawFaceNormals(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces, bool flip);

	void drawModel(const std::vector<Face>& faces, const std::vector<glm::vec3>&, const glm::vec4& color,
		const std::vector<std::shared_ptr<Light>>& lights, const std::vector<glm::vec3>& normals,
		float KA, float KD, float KS, float sExp, const glm::vec3& cameraPos, const glm::mat4& mat, const Scene&);

	void getBaryW(float i, float j, float s6, float s5, float s1, float s2, float s3, float s4, float denominator, float* w);
	glm::vec4 getPosColor(float i, float j, float z, const glm::vec3& cameraPos, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3, const std::vector<std::shared_ptr<Light>>& lights, bool rainbow, bool circles, const glm::vec4& color, float KA, float KD, float KS, float sExp, float w1, float w2, float w3, const glm::mat4& volMat, int shadingType);

	std::vector<glm::vec3> applyTransfrom(const std::vector<glm::vec3>& ver, const glm::mat4& mat);

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	void drawLine(float x1, float y1, float z1, float x2, float y2, float z2, glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	void drawBounding(const std::vector<glm::vec3>& vertices, const glm::vec4& color);
	void toggleAliasing();
};
