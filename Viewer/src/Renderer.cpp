#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
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
	for (int x = 0; x < viewportWidth; x++) {
		for (int y = 0; y < viewportHeight; y++) {
			zBuffer[ZINDEX(viewportWidth, x, y)] = std::numeric_limits<float>::max();
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++) {
		for (int j = 0; j < viewportHeight; j++) {
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
	if (activeCamera != -1) {
		viewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getViewTransformation();
		worldViewMatrix = scene.getCamera(scene.GetActiveCameraIndex())->getWorldViewTransformation();
		projection = scene.getCamera(scene.GetActiveCameraIndex())->getProjection();
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
		points = applyTransfrom(points, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 1)));
		normals = applyTransfrom(normals, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 1)));
		this->drawModel(model->getFaces(), points, model->GetColor(), scene.getRainbow());
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
			boundPoints = applyTransfrom(boundPoints, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 1)));
			this->drawBounding(boundPoints, model->GetColor());
		}
	}

	for (int i = 0; i < scene.GetCameraCount(); i++) {
		if (i != scene.GetActiveCameraIndex()) {
			auto camera = scene.getCamera(i);
			auto points = applyTransfrom(camera->getVertices(), camera->GetObjectTransformation());
			points = applyTransfrom(points, camera->GetWorldTransformation());
			points = applyTransfrom(points, worldViewMatrix);
			points = applyTransfrom(points, viewMatrix);
			points = applyTransfrom(points, projection);
			points = applyTransfrom(points, Utils::getTranslationMatrix(glm::vec3(1, 1, 0)));
			points = applyTransfrom(points, Utils::getScaleMatrix(glm::vec3(viewportWidth / 2, viewportHeight / 2, 1)));
			this->drawModel(camera->getFaces(), points, camera->GetColor());
		}
	}
}


void Renderer::drawModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, glm::vec4 color, bool rainbow) {
	for (auto face : faces) {
		glm::vec3 v0, v1, v2;		// the 3 points that make the triangle
		v0 = vertices[face.GetVertexIndex(0)];
		v1 = vertices[face.GetVertexIndex(1)];
		v2 = vertices[face.GetVertexIndex(2)];
		if (rainbow) {
			color = glm::vec4((float)((int)(v0.x * 2 + v0.y * 3 + v0.z * 4) % 256) / 256, (float)((int)(v1.x * 3 + v1.y * 4 + v1.z * 5) % 256) / 256, (float)((int)(v2.x * 4 + v2.y * 5 + v2.z * 6) % 256) / 256, 1);
		}
		int top = (int) std::max(v2.y, std::max(v0.y, v1.y));
		int bottom = (int) std::min(v2.y, std::min(v0.y, v1.y));
		int right = (int) std::max(v2.x, std::max(v0.x, v1.x));
		int left = (int) std::min(v2.x, std::min(v0.x, v1.x));
		float s1 = v2.y - v0.y;
		float s2 = v2.x - v0.x;
		float s3 = v1.y - v0.y;
		for (int i = left-1; i < right+1; ++i) {
			for (int j = bottom-1; j < top+1; ++j) {
				float s4 = j - v0.y;
				float w1 = (v0.x *s1 + s4 * s2 - i * s1) / (s3 *s2 - (v1.x - v0.x)*s1);
				float w2 = (s4 - w1 * s3) / s1;
				if ((w1 >= 0) && (w2 >= 0) && (w1 + w2 <= 1)) {
					float z = v0.z + w2 * (v2.z - v0.z) + w1 * (v1.z - v0.z);
					putPixel(i, j, z, color);
				}
			}
		}
	}
}

void Renderer::drawNormals(std::vector<glm::vec3> vertices, std::vector<Face> faces, std::vector<glm::vec3> normals, bool flip)
{
	for (auto face : faces) {
		glm::vec3 v0, v1, v2;
		glm::vec3 n0, n1, n2;
		v0 = vertices[face.GetVertexIndex(0)];
		v1 = vertices[face.GetVertexIndex(1)];
		v2 = vertices[face.GetVertexIndex(2)];
		n0 = normals[face.GetVertexIndex(0)];
		n1 = normals[face.GetVertexIndex(1)];
		n2 = normals[face.GetVertexIndex(2)];
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

void Renderer::drawFaceNormals(std::vector<glm::vec3> vertices, std::vector<Face> faces, bool flip) {
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

void Renderer::drawBounding(std::vector<glm::vec3> v, glm::vec4 color) {
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