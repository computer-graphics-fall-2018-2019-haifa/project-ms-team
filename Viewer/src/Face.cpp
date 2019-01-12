#include "Face.h"

Face::Face(std::istream& issLine)
{
	vertexIndices =  { 0, 0, 0 };
	normalIndices =  { 0, 0, 0 };
	textureIndices = { 0, 0, 0 };

	char c;
	for (int i = 0; i < 3; i++)
	{
		issLine >> std::ws >> vertexIndices.at(i) >> std::ws;

		if (issLine.peek() != '/')
		{
			continue;
		}

		issLine >> c >> std::ws;

		if (issLine.peek() == '/')
		{
			issLine >> c >> std::ws >> normalIndices.at(i);
			continue;
		}
		else
		{
			issLine >> textureIndices.at(i);
		}

		if (issLine.peek() != '/')
		{
			continue;
		}

		issLine >> c >> normalIndices.at(i);
	}
}

Face::Face(int v1, int v2, int v3, int t1, int t2, int t3, int n1, int n2, int n3)
{
	vertexIndices = { 0, 0, 0 };
	normalIndices = { 0, 0, 0 };
	textureIndices = { 0, 0, 0 };

	vertexIndices[0] = v1;
	vertexIndices[1] = v2;
	vertexIndices[2] = v3;

	normalIndices[0] = n1;
	normalIndices[1] = n2;
	normalIndices[2] = n3;

	textureIndices[0] = t1;
	textureIndices[1] = t2;
	textureIndices[2] = t3;
}

Face::~Face()
{

}

const int Face::GetVertexIndex(int index)
{
	return vertexIndices[index] - 1;	// decrement by 1 because the index in the file is one larger then the actual index in the vector
}

const int Face::GetNormalIndex(int index)
{
	return normalIndices[index] - 1;	// decrement by 1 because the index in the file is one larger then the actual index in the vector
}

const int Face::GetTextureIndex(int index)
{
	return textureIndices[index] - 1;	// decrement by 1 because the index in the file is one larger then the actual index in the vector
}
