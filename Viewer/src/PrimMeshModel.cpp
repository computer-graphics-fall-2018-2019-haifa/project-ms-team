#include "PrimMeshModel.h"



PrimMeshModel::PrimMeshModel()
{
}

MeshModel PrimMeshModel::genCube()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex;
	std::vector<Face> faces;

	vertices.push_back(glm::vec3(-1.0f, 0.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 0.0f, -1.0f));
	vertices.push_back(glm::vec3(1.0f, 0.0f, -1.0f));
	vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));

	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

	tex.push_back(glm::vec2(1.0f, 0.0f));
	tex.push_back(glm::vec2(1.0f, 1.0f));
	tex.push_back(glm::vec2(0.0f, 1.0f));
	tex.push_back(glm::vec2(0.0f, 0.0f));

	faces.push_back(Face(1, 2, 3, 1, 2, 3, 1, 1, 1));
	faces.push_back(Face(3, 4, 1, 3, 4, 1, 1, 1, 1));
	faces.push_back(Face(5, 6, 7, 4, 1, 2, 2, 2, 2));
	faces.push_back(Face(7, 8, 5, 2, 3, 4, 2, 2, 2));
	faces.push_back(Face(1, 4, 5, 4, 1, 2, 3, 3, 3));
	faces.push_back(Face(6, 5, 1, 2, 3, 4, 3, 3, 3));
	faces.push_back(Face(4, 3, 7, 4, 1, 2, 4, 4, 4));
	faces.push_back(Face(7, 6, 4, 2, 3, 4, 4, 4, 4));
	faces.push_back(Face(3, 2, 8, 4, 1, 2, 5, 5, 5));
	faces.push_back(Face(8, 7, 3, 2, 3, 4, 5, 5, 5));
	faces.push_back(Face(2, 1, 5, 4, 1, 2, 6, 6, 6));
	faces.push_back(Face(5, 8, 2, 2, 3, 4, 6, 6, 6));

	return MeshModel(faces, vertices, Utils::CalculateNormals(vertices, faces), tex, "Prim Cube");
}

MeshModel PrimMeshModel::genPyramid()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex;
	std::vector<Face> faces;

	vertices.push_back(glm::vec3(0.942f, 0.0f, -0.333f));
	vertices.push_back(glm::vec3(-0.471f, 0.471f, -0.333f));
	vertices.push_back(glm::vec3(-0.471f, -0.471f, -0.333f));
	vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	normals.push_back(glm::vec3(0.942f, 0.0f, -0.333f));
	normals.push_back(glm::vec3(-0.471f, 0.471f, -0.333f));
	normals.push_back(glm::vec3(-0.471f, -0.471f, -0.333f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	tex.push_back(glm::vec2(1.0f, 0.0f));
	tex.push_back(glm::vec2(1.0f, 1.0f));
	tex.push_back(glm::vec2(0.0f, 1.0f));
	tex.push_back(glm::vec2(0.0f, 0.0f));

	faces.push_back(Face(1, 2, 3, 1, 2, 3, 1, 2, 3));
	faces.push_back(Face(1, 2, 4, 1, 2, 4, 1, 2, 4));
	faces.push_back(Face(1, 3, 4, 1, 3, 4, 1, 3, 4));
	faces.push_back(Face(2, 3, 4, 2, 3, 4, 2, 3, 4));

	return MeshModel(faces, vertices, Utils::CalculateNormals(vertices, faces), tex, "Prim Pyramid");
}

PrimMeshModel::~PrimMeshModel()
{
}
