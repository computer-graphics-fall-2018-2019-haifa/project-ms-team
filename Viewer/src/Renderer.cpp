#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define ZINDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr),
	aliasing(false)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
}

Renderer::~Renderer()
{
	if (colorBuffer) {
		delete[] colorBuffer;
	}
	if (zBuffer) {
		delete[] zBuffer;
	}
}

void Renderer::putPixel(int i, int j, float z, const glm::vec3& color) {
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;
	if (zBuffer[ZINDEX(viewportWidth, i, j)] > z) {
		colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
		colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
		colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
		zBuffer[ZINDEX(viewportWidth, i, j)] = z;
	}
}

void Renderer::drawLine(float x1, float y1, float z1, float x2, float y2, float z2, glm::vec4 color) {
	if (x2 - x1 == 0) {			// need to draw a vertical line
		if (y1 >= y2) {			// switch y1 and y2 if needed
			std::swap(y1, y2);
		}
		while (y1 <= y2) {
			this->putPixel((int)x1, (int)y1, z1, color);
			y1++;
		}
		return;
	}
	if (x2 - x1 < 0) {		// we want to always draw the line to the right, so if x2 is smaller then x1, switch the points
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	float deltaX = x2 - x1;
	float deltaY = y2 - y1;
	float slope = deltaY / deltaX;
	deltaX *= 2;		// scaling now to not perform it inside the loop each time
	deltaY *= 2;
	if (slope < 0) {	// case of a line with 'downwards' slope
		if (slope < -1) {		// case of a line that decreases on Y faster then it increases on X
			float e = deltaY;
			while (y1 >= y2) {
				if (e > 0) {
					x1++; e += deltaY;
				}
				this->putPixel((int)x1, (int)y1, z1, color);
				y1--; e += deltaX;
			}
		}
		else {					// case of a line that decreases on Y slower then it increases on X
			float e = -deltaX;
			while (x1 <= x2) {
				if (e > 0) {
					y1--; e -= deltaX;
				}
				this->putPixel((int)x1, (int)y1, z1, color);
				x1++; e -= deltaY;
			}
		}
	}
	else {				// case of a line with 'upwards' slope
		if (slope > 1) {		// case of a line that increases on Y faster then it increases on X
			float e = -deltaY;
			while (y1 <= y2) {
				if (e > 0) {
					x1++; e -= deltaY;
				}
				this->putPixel((int)x1, (int)y1, z1, color);
				y1++; e += deltaX;
			}
		}
		else {					// case of a line that increases on Y slower then it increases on X
			float e = -deltaX;
			while (x1 <= x2) {
				if (e > 0) {
					y1++; e -= deltaX;
				}
				this->putPixel((int)x1, (int)y1, z1, color);
				x1++; e += deltaY;
			}
		}
	}
}

void Renderer::toggleAliasing()
{
	this->aliasing = !this->aliasing;
}

void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{
	if (colorBuffer) {
		delete[] colorBuffer;
	}
	if (zBuffer) {
		delete[] zBuffer;
	}
	colorBuffer = new float[3 * viewportWidth * viewportHeight];
	zBuffer = new float[viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++) {
		for (int y = 0; y < viewportHeight; y++) {
			putPixel(x, y, -1, glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++) {
		for (int j = 0; j < viewportHeight; j++) {
			zBuffer[ZINDEX(viewportWidth, i, j)] = std::numeric_limits<float>::max();
			colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
			colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
			colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
		}
	}
}

void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	if (this->aliasing) {
		this->viewportWidth *= 2;
		this->viewportHeight *= 2;
	}
	createBuffers(this->viewportWidth, this->viewportHeight);
	createOpenGLBuffer();
}

void Renderer::Render(const Scene& scene) {
	int activeCamera = scene.GetActiveCameraIndex();
	glm::mat4 viewMatrix(1);
	glm::mat4 worldViewMatrix(1);
	glm::mat4 projection(1);
	glm::mat4 middleTranslate = Utils::getTranslationMatrix(glm::vec3(1, 1, 0));
	glm::mat4 scaleWindow = Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 10));

	glm::vec3 cameraPos(0, 0, 1);
	auto lights = scene.getLights();
	if (activeCamera != -1) {
		viewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getViewTransformation();
		worldViewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getWorldViewTransformation();
		projection = scene.getCamera(scene.GetActiveCameraIndex())->getProjection();
		cameraPos = scene.getCamera(scene.GetActiveCameraIndex())->getPosition();
	}
	
	glm::mat4 totalMat = scaleWindow * (middleTranslate * (projection * (viewMatrix * worldViewMatrix)));

	for (int i = 0; i < scene.GetModelCount(); i++) {
		auto model = scene.getModel(i);
		glm::mat4 modelTransform = totalMat * (model->GetWorldTransformation() * model->GetObjectTransformation());
		auto points = applyTransfrom(model->getVertices(), modelTransform);
		auto normals = applyTransfrom(model->getNormals(), modelTransform);
		this->drawModel(model->getFaces(), points, model->GetColor(), lights, normals, model->getKAmbient(), model->getKDiffuse(), model->getKSpecular(), model->getSpecularExp(), cameraPos, totalMat, scene);
		if (model->isDrawNormals()) {
			this->drawNormals(points, model->getFaces(), normals, model->isFlipNormals());
		}
		if (model->isDrawFaceNormals()) {
			this->drawFaceNormals(points, model->getFaces(), model->isFlipFaceNormals());
		}

		if (model->isDrawBounding()) {
			auto boundPoints = applyTransfrom(model->getBoundingVer(), model->GetObjectTransformation());
			boundPoints = applyTransfrom(boundPoints, model->GetWorldTransformation());
			boundPoints = applyTransfrom(boundPoints, totalMat);
			this->drawBounding(boundPoints, model->GetColor());
		}
	}

	for (int i = 0; i < scene.GetCameraCount(); i++) {
		if (i != scene.GetActiveCameraIndex()) {
			auto camera = scene.getCamera(i);
			glm::mat4 cameraTransform = totalMat * (camera->GetWorldTransformation() * camera->GetObjectTransformation());
			auto points = applyTransfrom(camera->getVertices(), cameraTransform);
			auto normals = applyTransfrom(camera->getNormals(), cameraTransform);
			this->drawModel(camera->getFaces(), points, camera->GetColor(), lights, normals, camera->getKAmbient(), camera->getKDiffuse(), camera->getKSpecular(), camera->getSpecularExp(), cameraPos, totalMat, scene);
		}
	}
}


void Renderer::drawModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const glm::vec4& color, const std::vector<std::shared_ptr<Light>>& lights, const std::vector<glm::vec3>& normals, float KA, float KD, float KS, float sExp, const glm::vec3& cameraPos, const glm::mat4& mat, const Scene& scene) {
	for (auto face : faces) {
		glm::vec3 v1, v2, v3;		// the 3 points that make the triangle
		v1 = vertices[face.GetVertexIndex(0)];
		v2 = vertices[face.GetVertexIndex(1)];
		v3 = vertices[face.GetVertexIndex(2)];
		glm::vec3 n1, n2, n3;
		n1 = normals[face.GetNormalIndex(0)];
		n2 = normals[face.GetNormalIndex(1)];
		n3 = normals[face.GetNormalIndex(2)];
		int top = (int) std::max(v2.y, std::max(v3.y, v1.y));
		int bottom = (int) std::min(v2.y, std::min(v3.y, v1.y));
		int right = (int) std::max(v2.x, std::max(v3.x, v1.x));
		int left = (int) std::min(v2.x, std::min(v3.x, v1.x));
		float s1 = v2.y - v3.y;
		float s2 = v3.y - v1.y;
		float s3 = v3.x - v2.x;
		float s4 = v1.x - v3.x;
		float denominator = (s1 * s4 + s3 * -s2);
		for (int x = left-1; x < right+1; ++x) {
			for (int y = bottom-1; y < top+1; ++y) {
				glm::vec4 posColor(0);
				float i = (float) x;
				float j = (float) y;
				float w0[3] = { 0.0f, 0.0f, 0.0f };
				getBaryW(i, j, i - v3.x, j - v3.y, s1, s2, s3, s4, denominator, w0);
				float w10 = w0[0];
				float w20 = w0[1];
				float w30 = w0[2];
				float trueZ = w10 * v1.z + w20 * v2.z + w30 * v3.z;
				if ((w10 >= 0) && (w20 >= 0) && (w30 >= 0)) {
					posColor = getPosColor(i, j, trueZ, cameraPos, v1, v2, v3, n1, n2, n3, lights, scene.getRainbow(), scene.getCircles(), color, KA, KD, KS, sExp, w10, w20, w30, mat, scene.getShadingType());
					scene.applyFog(posColor, trueZ);
					putPixel(x, y, trueZ, posColor);
				}
			}
		}
	}
}

void Renderer::getBaryW(float i, float j, float s6, float s5, float s1, float s2, float s3, float s4, float denominator, float* w) {
	w[0] = (s1 * s6 + s3 * s5) / denominator;
	w[1] = (s2 * s6 + s4 * s5) / denominator;
	w[2] = 1 - w[1] - w[0];
}

glm::vec4 Renderer::getPosColor(float i, float j, float z, const glm::vec3& cameraPos, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3,
	const std::vector<std::shared_ptr<Light>>& lights, bool rainbow, bool circles, const glm::vec4& color, float KA, float KD, float KS, float sExp, float w1, float w2, float w3, const glm::mat4& volMat, int shadingType) {
	glm::vec4 finalColor(0);
	glm::vec4 pColor(color);
	glm::vec3 normalCamera = cameraPos - glm::vec3(i, j, z);
	glm::vec3 pNormal(0.0f, 0.0f, 0.0f);
	switch (shadingType) {
	case(0):		// flat
		pNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
		break;
	case(1):		// gouraud
		break;
	case(2):		// phong
		pNormal = w1 * n1 + w2 * n2 + w3 * n3;
		break;
	default:
		pNormal = glm::normalize(glm::cross(w2 * v2 - w1 * v1, w3 * v3 - w1 * v1));
	}
	if (shadingType != 1) {
		for (auto light : lights) {
			if (rainbow) {
				pColor = glm::vec4((float)((int)(i * w1 + j * w1 + z * w1) % 256) / 256, (float)((int)(i * w2 + j * w2 + z * w2) % 256) / 256, (float)((int)(i * w3 + j * w3 + z * w3) % 256) / 256, 1);
			}
			if (circles) {
				KA = (float)((int)(i * i + j * j) % 256) / 256;
				KD = (float)((int)(j * j + z * z) % 256) / 256;
				KS = (float)((int)(i * i + z * z) % 256) / 256;
			}
			float theta = 0.0f;
			glm::vec4 lightColor = light->GetColor();
			glm::vec4 normalColor(pColor.x * lightColor.x, pColor.y * lightColor.y, pColor.z * lightColor.z, pColor.w * lightColor.w);
			switch (light->getType()) {
			case(0):
				finalColor += KA * normalColor;
				break;
			case(1):
				auto lDir = light->getDirection(volMat);
				theta = glm::clamp((glm::dot(pNormal, lDir) / (glm::length(lDir) * glm::length(pNormal))), 0.0f, 1.0f);
				finalColor += KD * theta * normalColor;
				break;
			case(2):
				glm::vec3 lightPos = light->getLightPos(volMat);
				glm::vec3 lightDirection = glm::vec3(lightPos.x - i, lightPos.y - j, lightPos.z - z);
				glm::vec3 reflection = glm::reflect(lightDirection, pNormal);
				theta = glm::pow(glm::dot(reflection, normalCamera) / (glm::length(reflection) * glm::length(normalCamera)), sExp);
				theta = glm::clamp(theta, 0.0f, 1.0f);
				finalColor += KS * theta * normalColor;
				break;
			default:
				finalColor = normalColor;
			}
		}
	}
	else if (shadingType == 1) {
		for (auto light : lights) {
			if (rainbow) {
				pColor = glm::vec4((float)((int)(i * w1 + j * w1 + z * w1) % 256) / 256, (float)((int)(i * w2 + j * w2 + z * w2) % 256) / 256, (float)((int)(i * w3 + j * w3 + z * w3) % 256) / 256, 1);
			}
			if (circles) {
				KA = (float)((int)(i * i + j * j) % 256) / 256;
				KD = (float)((int)(j * j + z * z) % 256) / 256;
				KS = (float)((int)(i * i + z * z) % 256) / 256;
			}
			glm::vec4 v1c(0.0f), v2c(0.0f), v3c(0.0f);		// colors at the vertixes
			float theta = 0.0f;
			glm::vec4 lightColor = light->GetColor();
			glm::vec4 normalColor(pColor.x * lightColor.x, pColor.y * lightColor.y, pColor.z * lightColor.z, pColor.w * lightColor.w);
			switch (light->getType()) {
			case(0):
				v1c = KA * normalColor;
				v2c = KA * normalColor;
				v3c = KA * normalColor;
				break;
			case(1):
				auto lDir = light->getDirection(volMat);
				theta = glm::clamp(glm::dot(n1, lDir) / (glm::length(lDir) * glm::length(n1)), 0.0f, 1.0f);
				v1c = KD * theta * normalColor;
				theta = glm::clamp(glm::dot(n2, lDir) / (glm::length(lDir) * glm::length(n2)), 0.0f, 1.0f);
				v2c = KD * theta * normalColor;
				theta = glm::clamp(glm::dot(n3, lDir) / (glm::length(lDir) * glm::length(n3)), 0.0f, 1.0f);
				v3c = KD * theta * normalColor;
				break;
			case(2):
				glm::vec3 lightPos = light->getLightPos(volMat);
				glm::vec3 lightDirection = glm::vec3(lightPos.x - i, lightPos.y - j, lightPos.z - z);
				glm::vec3 reflection1 = glm::reflect(lightDirection, n1);
				glm::vec3 reflection2 = glm::reflect(lightDirection, n2);
				glm::vec3 reflection3 = glm::reflect(lightDirection, n3);
				theta = glm::clamp(glm::pow(glm::dot(reflection1, normalCamera) / (glm::length(reflection1) * glm::length(normalCamera)), sExp), 0.0f, 1.0f);
				v1c = KS * theta * normalColor;
				theta = glm::clamp(glm::pow(glm::dot(reflection2, normalCamera) / (glm::length(reflection2) * glm::length(normalCamera)), sExp), 0.0f, 1.0f);
				v2c = KS * theta * normalColor;
				theta = glm::clamp(glm::pow(glm::dot(reflection3, normalCamera) / (glm::length(reflection3) * glm::length(normalCamera)), sExp), 0.0f, 1.0f);
				v3c = KS * theta * normalColor;
				break;
			default:
				v1c = normalColor;
				v2c = normalColor;
				v3c = normalColor;
			}
			finalColor += w1 * v1c + w2 * v2c + w3 * v3c;
		}
	}
	return glm::clamp(finalColor, 0.0f, 1.0f);
}


void Renderer::drawNormals(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces, const std::vector<glm::vec3>& normals, bool flip)
{
	for (auto face : faces) {
		glm::vec3 v0, v1, v2;
		glm::vec3 n0, n1, n2;
		v0 = vertices[face.GetVertexIndex(0)];
		v1 = vertices[face.GetVertexIndex(1)];
		v2 = vertices[face.GetVertexIndex(2)];
		n0 = normals[face.GetNormalIndex(0)];
		n1 = normals[face.GetNormalIndex(1)];
		n2 = normals[face.GetNormalIndex(2)];
		if (flip) {
			this->drawLine(v0.x, v0.y, v0.z, 2 * v0.x - n0.x, 2 * v0.y - n0.y, 2 * v0.z - n0.z, glm::vec4(1, 0, 1, 1));
			this->drawLine(v1.x, v1.y, v1.z, 2 * v1.x - n1.x, 2 * v1.y - n1.y, 2 * v0.z - n1.z, glm::vec4(1, 0, 1, 1));
			this->drawLine(v2.x, v2.y, v2.z, 2 * v2.x - n2.x, 2 * v2.y - n2.y, 2 * v0.z - n2.z, glm::vec4(1, 0, 1, 1));
		}
		else {
			this->drawLine(v0.x, v0.y, v0.z, n0.x, n0.y, n0.z, glm::vec4(0, 1, 0, 1));
			this->drawLine(v1.x, v1.y, v1.z, n1.x, n1.y, n1.z, glm::vec4(0, 1, 0, 1));
			this->drawLine(v2.x, v2.y, v2.z, n2.x, n2.y, n2.z, glm::vec4(0, 1, 0, 1));
		}
	}
}

void Renderer::drawFaceNormals(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces, bool flip) {
	for (auto face : faces) {
		glm::vec3 v0, v1, v2;
		v0 = vertices[face.GetVertexIndex(0)];
		v1 = vertices[face.GetVertexIndex(1)];
		v2 = vertices[face.GetVertexIndex(2)];
		float x = (v0.x + v1.x + v2.x) / 3;
		float y = (v0.y + v1.y + v2.y) / 3;
		float z = (v0.z + v1.z + v2.z) / 3;
		glm::vec3 u, v;
		u = v1 - v0;
		v = v2 - v0;
		if (flip) {
			glm::vec3 normal = glm::cross(u, v);
			this->drawLine(x, y, z, x + normal.x, y + normal.y, z + normal.z, glm::vec4(0, 0, 1, 1));
		}
		else {
			glm::vec3 normal = glm::cross(v, u);
			this->drawLine(x, y, z, x + normal.x, y + normal.y, z + normal.z, glm::vec4(1, 1, 0, 1));
		}
	}
}


void Renderer::drawBounding(const std::vector<glm::vec3>& v, const glm::vec4& color) {
	this->drawLine(v[0].x, v[0].y, v[0].z, v[1].x, v[1].y, v[1].z, color);
	this->drawLine(v[0].x, v[0].y, v[0].z, v[2].x, v[2].y, v[2].z, color);
	this->drawLine(v[0].x, v[0].y, v[0].z, v[4].x, v[4].y, v[4].z, color);
	this->drawLine(v[1].x, v[1].y, v[1].z, v[3].x, v[3].y, v[3].z, color);
	this->drawLine(v[1].x, v[1].y, v[1].z, v[5].x, v[5].y, v[5].z, color);
	this->drawLine(v[2].x, v[2].y, v[2].z, v[3].x, v[3].y, v[3].z, color);
	this->drawLine(v[2].x, v[2].y, v[2].z, v[6].x, v[6].y, v[6].z, color);
	this->drawLine(v[3].x, v[3].y, v[3].z, v[7].x, v[7].y, v[7].z, color);
	this->drawLine(v[4].x, v[4].y, v[4].z, v[5].x, v[5].y, v[5].z, color);
	this->drawLine(v[4].x, v[4].y, v[4].z, v[6].x, v[6].y, v[6].z, color);
	this->drawLine(v[5].x, v[5].y, v[5].z, v[7].x, v[7].y, v[7].z, color);
	this->drawLine(v[6].x, v[6].y, v[6].z, v[7].x, v[7].y, v[7].z, color);
}

std::vector<glm::vec3> Renderer::applyTransfrom(const std::vector<glm::vec3>& ver, const glm::mat4& mat) {
	std::vector<glm::vec3> new_ver;
	for (auto v : ver) {
		glm::vec4 temp(v.x, v.y, v.z, 1);
		temp = mat * temp;
		new_ver.push_back(glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w));
	}
	return new_ver;
}
//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// malloc for a texture on the gpu.
	if (this->aliasing) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth / 2, viewportHeight / 2, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, viewportWidth / 2, viewportHeight / 2);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, viewportWidth, viewportHeight);
	}
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	if (this->aliasing) {
		int tw = viewportWidth / 2;
		int th = viewportHeight / 2;
		float* cb = new float[3 * tw * th];
		for (int x = 0; x < tw; ++x) {
			for (int y = 0; y < th; ++y) {
				cb[INDEX(tw, x, y, 0)] = (colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y, 0)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y, 0)] + colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y + 1, 0)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y + 1, 0)]) * 0.25f;
				cb[INDEX(tw, x, y, 1)] = (colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y, 1)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y, 1)] + colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y + 1, 1)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y + 1, 1)]) * 0.25f;
				cb[INDEX(tw, x, y, 2)] = (colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y, 2)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y, 2)] + colorBuffer[INDEX(viewportWidth, 2 * x, 2 * y + 1, 2)] + colorBuffer[INDEX(viewportWidth, 2 * x + 1, 2 * y + 1, 2)]) * 0.25f;
			}
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth / 2, viewportHeight / 2, GL_RGB, GL_FLOAT, cb);
		delete[] cb;
	}
	else {
		// memcopy's colorBuffer into the gpu.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);
	}
	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}