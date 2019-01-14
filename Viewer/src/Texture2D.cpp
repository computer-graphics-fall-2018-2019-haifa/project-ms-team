#include "Texture2D.h"
#include <iostream>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Texture2D::Texture2D()
	: mTexture(0)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTexture);
}

//-----------------------------------------------------------------------------
// Load a texture with a given filename using stb image loader
// http://nothings.org/stb_image.h
// Creates mip maps if generateMipMaps is true.
//-----------------------------------------------------------------------------
bool Texture2D::loadTexture(const string& fileName, bool generateMipMaps)
{
	int width, height, components;
	glDeleteTextures(1, &mTexture); // clean old textures
	// Use stbi image library to load our image
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);

	if (imageData == NULL)
	{
		std::cerr << "Error loading texture '" << fileName << "'" << std::endl;
		return false;
	}

	// Invert image
	int widthInBytes = width * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = height / 2;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageData + row * widthInBytes;
		bottom = imageData + (height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture); // all upcoming GL_TEXTURE_2D operations will affect our texture object (mTexture)

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	// GL_CLAMP_TO_EDGE
	// GL_REPEAT
	// GL_MIRRORED_REPEAT
	// GL_CLAMP_TO_BORDER
	// GL_LINEAR
	// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we don't accidentally mess up our mTexture

	return true;
}

//-----------------------------------------------------------------------------
// Bind the texture unit passed in as the active texture in the shader
//-----------------------------------------------------------------------------
void Texture2D::bind(GLuint texUnit) const
{
	assert(texUnit >= 0 && texUnit < 32);

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

//-----------------------------------------------------------------------------
// Unbind the texture unit passed in as the active texture in the shader
//-----------------------------------------------------------------------------
void Texture2D::unbind(GLuint texUnit) const
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::genRandomTexture(bool generateMipMaps)
{
	glDeleteTextures(1, &mTexture); // clean old textures

	float * image = new float[512 * 512 * 3];
	//unsigned char imageData[256][256][3];
	srand(time(NULL));

	for (int iter = 0; iter < 5000; ++iter) {
		int centerX = rand() % 512;
		int centerY = rand() % 512;

		int radius = rand() % 20;

		float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		for (int i = std::max(0, centerX - radius); i < std::min(512, centerX + radius); ++i) {
			for (int j = std::max(0, centerY - radius); j < std::min(512, centerY + radius); ++j) {
				float dist = std::sqrt((i - centerX)*(i - centerX) + (j - centerY)*(j - centerY));
				if (dist < radius) {
					image[3 * (i * 512 + j) + 0] = r;
					image[3 * (i * 512 + j) + 1] = g;
					image[3 * (i * 512 + j) + 2] = b;
				}
			}
		}
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture); // all upcoming GL_TEXTURE_2D operations will affect our texture object (mTexture)

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	// GL_CLAMP_TO_EDGE
	// GL_REPEAT
	// GL_MIRRORED_REPEAT
	// GL_CLAMP_TO_BORDER
	// GL_LINEAR
	// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we don't accidentally mess up our mTexture

	return true;
}
