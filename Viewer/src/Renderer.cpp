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
	glm::vec3 cameraPos(0, 0, 1);
	auto lights = scene.getLights();
	if (activeCamera != -1) {
		viewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getViewTransformation();
		worldViewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getWorldViewTransformation();
		projection = scene.getCamera(scene.GetActiveCameraIndex())->getProjection();
		cameraPos = scene.getCamera(scene.GetActiveCameraIndex())->getPosition();
	}
	
	for (int i = 0; i < scene.GetModelCount(); i++) {
		auto model = scene.getModel(i);
		auto points = applyTransfrom(model->getVertices(), model->GetObjectTransformation());
		auto normals = applyTransfrom(model->getNormals(), model->GetObjectTransformation());
		points = applyTransfrom(points, model->GetWorldTransformation());
		normals = applyTransfrom(normals, model->GetWorldTransformation());
		points = applyTransfrom(points, worldViewMatrix);
		normals = applyTransfrom(normals, worldViewMatrix);
		points = applyTransfrom(points, viewMatrix);
		normals = applyTransfrom(normals, viewMatrix);
		points = applyTransfrom(points, projection);
		normals = applyTransfrom(normals, projection);
		points = applyTransfrom(points, Utils::getTranslationMatrix(glm::vec3(1,1,0)));
		normals = applyTransfrom(normals, Utils::getTranslationMatrix(glm::vec3(1,1,0)));
		points = applyTransfrom(points, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100)));
		normals = applyTransfrom(normals, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100)));
		this->drawModel(model->getFaces(), points, model->GetColor(), lights, normals, model->getKAmbient(), model->getKDiffuse(), model->getKSpecular(), cameraPos, scene.getRainbow(), scene.getCircles());
		if (model->isDrawNormals()) {
			this->drawNormals(points, model->getFaces(), normals, model->isFlipNormals());
		}
		if (model->isDrawFaceNormals()) {
			this->drawFaceNormals(points, model->getFaces(), model->isFlipFaceNormals());
		}

		if (model->isDrawBounding()) {
			auto boundPoints = applyTransfrom(model->getBoundingVer(), model->GetObjectTransformation());
			boundPoints = applyTransfrom(boundPoints, model->GetWorldTransformation());
			boundPoints = applyTransfrom(boundPoints, worldViewMatrix);
			boundPoints = applyTransfrom(boundPoints, viewMatrix);
			boundPoints = applyTransfrom(boundPoints, projection);
			boundPoints = applyTransfrom(boundPoints, Utils::getTranslationMatrix(glm::vec3(1, 1, 0)));
			boundPoints = applyTransfrom(boundPoints, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100)));
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
			points = applyTransfrom(points, worldViewMatrix);
			normals = applyTransfrom(normals, worldViewMatrix);
			points = applyTransfrom(points, viewMatrix);
			normals = applyTransfrom(normals, viewMatrix);
			points = applyTransfrom(points, projection);
			normals = applyTransfrom(normals, projection);
			points = applyTransfrom(points, Utils::getTranslationMatrix(glm::vec3(1, 1, 0)));
			normals = applyTransfrom(normals, Utils::getTranslationMatrix(glm::vec3(1, 1, 0)));
			points = applyTransfrom(points, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100)));
			normals = applyTransfrom(normals, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 100)));
			this->drawModel(camera->getFaces(), points, camera->GetColor(), lights, normals, camera->getKAmbient(), camera->getKDiffuse(), camera->getKSpecular(), cameraPos);
		}
	}
}


void Renderer::drawModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const glm::vec4& color, const std::vector<std::shared_ptr<Light>>& lights, const std::vector<glm::vec3>& normals, float KA, float KD, float KS, const glm::vec3& cameraPos, bool rainbow, bool circles) {
	for (auto face : faces) {
		glm::vec3 v1, v2, v3;		// the 3 points that make the triangle
		glm::vec4 trueColor(color);
		v1 = vertices[face.GetVertexIndex(0)];
		v2 = vertices[face.GetVertexIndex(1)];
		v3 = vertices[face.GetVertexIndex(2)];
		glm::vec3 n1, n2, n3;
		n1 = normals[face.GetNormalIndex(0)] - v1;
		n2 = normals[face.GetNormalIndex(1)] - v2;
		n3 = normals[face.GetNormalIndex(2)] - v3;
		int top = (int) std::max(v2.y, std::max(v3.y, v1.y));
		int bottom = (int) std::min(v2.y, std::min(v3.y, v1.y));
		int right = (int) std::max(v2.x, std::max(v3.x, v1.x));
		int left = (int) std::min(v2.x, std::min(v3.x, v1.x));
		float s1 = v2.y - v3.y;
		float s2 = v3.y - v1.y;
		float s3 = v3.x - v2.x;
		float s4 = v1.x - v3.x;
		float denominator = (s1 * s4 + s3 * -s2);
		for (int i = left-1; i < right+1; ++i) {
			for (int j = bottom-1; j < top+1; ++j) {
				float s5 = j - v3.y;
				float s6 = i - v3.x;
				float w1 = (s1 * s6 + s3 * s5) / denominator;
				float w2 = (s2 * s6 + s4 * s5) / denominator;
				float w3 = 1 - w1 - w2;			//third lambada
				if ((w1 >= 0) && (w2 >= 0) && (w3 >= 0)) {
					float z = w1 * v1.z + w2 * v2.z + w3 * v3.z;
					glm::vec4 finalColor(0);
					glm::vec3 normalCamera = glm::normalize(cameraPos - glm::vec3(i, j, z));
					//glm::vec3 pNormal = (n1 * w1 + n2 * w2 + n3 * w3);
					glm::vec3 pNormal = glm::cross(v2 - v1, v3 - v1);
					//glm::vec3 pNormal = glm::cross(w2 * v2 - w1 * v1, w3 * v3 - w1 * v1);
					for (auto light : lights) {
						if (rainbow) {
							trueColor = glm::vec4((float)((int)(i * w1 + j * w1 + z * w1) % 256) / 256, (float)((int)(i * w2 + j * w2 + z * w2) % 256) / 256, (float)((int)(i * w3 + j * w3 + z * w3) % 256) / 256, 1);
						}
						if (circles) {
							trueColor = glm::vec4((float)((int)(i * i + i * j + i * z) % 256) / 256, (float)((int)(j * i + j * j + j * z) % 256) / 256, (float)((int)(z * i + z * j + z * z) % 256) / 256, 1);
						}
						float theta = 0.0f;
						glm::vec4 lightColor = light->GetColor();
						glm::vec4 normalColor(trueColor.x * lightColor.x, trueColor.y * lightColor.y, trueColor.z * lightColor.z, trueColor.w * lightColor.w);
						switch (light->getType()) {
						case(0):
							finalColor += KA * normalColor;
							break;
						case(1):
							theta = glm::clamp((glm::dot(pNormal, light->getDirection()))/ (glm::length(pNormal)*glm::length(light->getDirection())), 0.0f, 1.0f);
							finalColor += KD * theta * normalColor;
							break;
						case(2):
							glm::vec3 lightPos = light->getPosition();
							glm::vec3 lightDirection = glm::normalize(glm::vec3(lightPos.x - i, lightPos.y - j, lightPos.z - z));
							glm::vec3 reflection = glm::reflect(lightDirection, pNormal);
							theta = glm::dot(reflection, normalCamera);
							theta = glm::clamp(std::abs(theta), 0.0f, 1.0f);
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
					putPixel(i, j, z, finalColor);
				}
			}
		}
	}
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