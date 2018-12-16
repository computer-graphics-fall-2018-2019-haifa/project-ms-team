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
	zBuffer(nullptr)
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
	createBuffers(viewportWidth, viewportHeight);
	createOpenGLBuffer();
}

void Renderer::Render(const Scene& scene) {
	int activeCamera = scene.GetActiveCameraIndex();
	glm::mat4 viewMatrix(1);
	glm::mat4 worldViewMatrix(1);
	glm::mat4 projection(1);
	glm::mat4 middleTranslate = Utils::getTranslationMatrix(glm::vec3(1, 1, 0));
	glm::mat4 scaleWindow = Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100));

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
		auto points = applyTransfrom(model->getVertices(), model->GetObjectTransformation());
		auto normals = applyTransfrom(model->getNormals(), model->GetObjectTransformation());
		points = applyTransfrom(points, model->GetWorldTransformation());
		normals = applyTransfrom(normals, model->GetWorldTransformation());
		points = applyTransfrom(points, totalMat);
		normals = applyTransfrom(normals, totalMat);
		this->drawModel(model->getFaces(), points, model->GetColor(), lights, normals, model->getKAmbient(), model->getKDiffuse(), model->getKSpecular(), cameraPos, totalMat, scene.getRainbow(), scene.getCircles(), scene.getAliasing());
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
			auto points = applyTransfrom(camera->getVertices(), camera->GetObjectTransformation());
			auto normals = applyTransfrom(camera->getNormals(), camera->GetObjectTransformation());
			points = applyTransfrom(points, camera->GetWorldTransformation());
			normals = applyTransfrom(normals, camera->GetWorldTransformation());
			points = applyTransfrom(points, totalMat);
			normals = applyTransfrom(normals, totalMat);
			this->drawModel(camera->getFaces(), points, camera->GetColor(), lights, normals, camera->getKAmbient(), camera->getKDiffuse(), camera->getKSpecular(), cameraPos, totalMat);
		}
	}
}


void Renderer::drawModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const glm::vec4& color, const std::vector<std::shared_ptr<Light>>& lights, const std::vector<glm::vec3>& normals, float KA, float KD, float KS, const glm::vec3& cameraPos, const glm::mat4& mat, bool rainbow, bool circles, bool aliasing) {
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
				float putAny = 0;
				float i = (float) x;
				float j = (float) y;
				float w0[3] = { 0.0f, 0.0f, 0.0f };
				float w1[3] = { 0.0f, 0.0f, 0.0f };
				float w2[3] = { 0.0f, 0.0f, 0.0f };
				float w3[3] = { 0.0f, 0.0f, 0.0f };
				getBaryW(i, j, i - v3.x, j - v3.y, s1, s2, s3, s4, denominator, w0);
				getBaryW(i + 0.5f, j, i + 0.5f - v3.x, j - v3.y, s1, s2, s3, s4, denominator, w1);
				getBaryW(i, j + 0.5f, i - v3.x, j + 0.5f - v3.y, s1, s2, s3, s4, denominator, w2);
				getBaryW(i + 0.5f, j + 0.5f, i + 0.5f - v3.x, j + 0.5f - v3.y, s1, s2, s3, s4, denominator, w3);
				float w10 = w0[0]; float w11 = w1[0]; float w12 = w2[0]; float w13 = w3[0];
				float w20 = w0[1]; float w21 = w1[1]; float w22 = w2[1]; float w23 = w3[1];
				float w30 = w0[2]; float w31 = w1[2]; float w32 = w2[2]; float w33 = w3[2];
				float trueZ = w10 * v1.z + w20 * v2.z + w30 * v3.z;
				if ((w10 >= 0) && (w20 >= 0) && (w30 >= 0)) {
					float z = w10 * v1.z + w20 * v2.z + w30 * v3.z;
					posColor += getPosColor(i, j, z, cameraPos, v1, v2, v3, n1, n2, n3, lights, rainbow, circles, color, KA, KD, KS, w10, w20, w30, mat);
					putAny++;
				}
				if (aliasing) {
					if ((w11 >= 0) && (w21 >= 0) && (w31 >= 0)) {
						float z = w11 * v1.z + w21 * v2.z + w31 * v3.z;
						posColor += getPosColor(i, j, z, cameraPos, v1, v2, v3, n1, n2, n3, lights, rainbow, circles, color, KA, KD, KS, w11, w21, w31, mat);
						putAny++;
					}
					if ((w12 >= 0) && (w22 >= 0) && (w32 >= 0)) {
						float z = w12 * v1.z + w22 * v2.z + w32 * v3.z;
						posColor += getPosColor(i, j, z, cameraPos, v1, v2, v3, n1, n2, n3, lights, rainbow, circles, color, KA, KD, KS, w12, w22, w32, mat);
						putAny++;
					}
					if ((w13 >= 0) && (w23 >= 0) && (w33 >= 0)) {
						float z = w13 * v1.z + w23 * v2.z + w33 * v3.z;
						posColor += getPosColor(i, j, z, cameraPos, v1, v2, v3, n1, n2, n3, lights, rainbow, circles, color, KA, KD, KS, w13, w23, w33, mat);
						putAny++;
					}
				}
				if (putAny) {
					putPixel(x, y, trueZ, posColor * (1/putAny));
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
	const std::vector<std::shared_ptr<Light>>& lights, bool rainbow, bool circles, const glm::vec4& color, float KA, float KD, float KS, float w1, float w2, float w3, const glm::mat4& volMat) {
	glm::vec4 finalColor(0);
	glm::vec4 pColor(color);
	glm::vec3 normalCamera = glm::normalize(cameraPos - glm::vec3(i, j, z));
	glm::vec3 pNormal = glm::cross(v2 - v1, v3 - v1);								// this is for flat
	//glm::vec3 pNormal = glm::cross(w2 * v2 - w1 * v1, w3 * v3 - w1 * v1);			// this is interesting
	//glm::vec3 pNormal = w1 * n1 + w2 * n2 + w3 * n3;								// this is for phong
	for (auto light : lights) {
		if (rainbow) {
			pColor = glm::vec4((float)((int)(i * w1 + j * w1 + z * w1) % 256) / 256, (float)((int)(i * w2 + j * w2 + z * w2) % 256) / 256, (float)((int)(i * w3 + j * w3 + z * w3) % 256) / 256, 1);
		}
		if (circles) {
			pColor = glm::vec4((float)((int)(i * i + i * j + i * z) % 256) / 256, (float)((int)(j * i + j * j + j * z) % 256) / 256, (float)((int)(z * i + z * j + z * z) % 256) / 256, 1);
		}
		float theta = 0.0f;
		glm::vec4 lightColor = light->GetColor();
		glm::vec4 normalColor(pColor.x * lightColor.x, pColor.y * lightColor.y, pColor.z * lightColor.z, pColor.w * lightColor.w);
		switch (light->getType()) {
		case(0):
			finalColor += KA * normalColor;
			break;
		case(1):
			theta = glm::clamp((glm::dot(pNormal, light->getDirection(volMat))) / (glm::length(pNormal)*glm::length(light->getDirection(volMat))), 0.0f, 1.0f);
			finalColor += KD * theta * normalColor;
			break;
		case(2):
			glm::vec3 lightPos = light->getLightPos(volMat);
			glm::vec3 lightDirection = glm::normalize(glm::vec3(lightPos.x - i, lightPos.y - j, lightPos.z - z));
			glm::vec3 reflection = glm::reflect(lightDirection, pNormal);
			theta = glm::dot(reflection, normalCamera);
			theta = glm::clamp(theta, 0.0f, 1.0f);
			finalColor += KS * theta * normalColor;
			break;
		default:
			finalColor = normalColor;
		}
	}
	finalColor.x = std::min(finalColor.x, 1.0f);
	finalColor.y = std::min(finalColor.y, 1.0f);
	finalColor.z = std::min(finalColor.z, 1.0f);
	finalColor.w = std::min(finalColor.w, 1.0f);
	return finalColor;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}