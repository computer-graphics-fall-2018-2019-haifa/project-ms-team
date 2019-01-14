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
	normals.push_back(glm::vec3(0.0f, 0.0f, 0.471f));

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

MeshModel PrimMeshModel::genUniform(int sides)
{
	assert(sides > 4);
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;


	float angle = glm::radians(360.0f) / static_cast<float>(sides);
	std::vector<glm::vec3> vUp;
	std::vector<glm::vec3> vDown;
	//create main vertexes
	for (int i = 0; i < sides; ++i) {
		float x = glm::cos(i*angle);
		float z = glm::sin(i*angle);
		vertices.push_back(glm::vec3(x, 2.0f, z));
		vDown.push_back(glm::vec3(x, 0.0f, z));
	}
	vertices.insert(vertices.end(), vDown.begin(), vDown.end());
	vDown.clear();

	// create main faces
	for (int i = 0; i < sides; ++i) {
		int low0 = i - 1, low1 = i, low2 = i + 1;
		int top0, top1, top2;
		
		low0 = low0 < 0 ? sides - 1 : low0;
		low2 = low2 > sides - 1 ? 0 : low2;

		top0 = low0 + sides;
		top1 = low1 + sides;
		top2 = low2 + sides;

		faces.push_back(Face(low1, top0, top2));
		faces.push_back(Face(top1, low0, low2));
	}


	// create top and bottom extra vertices
	for (int i = 0; i < sides; ++i) {
		// line 1
		float x1 = vertices[i].x;
		float x2 = vertices[i + 3].x;
		float y1 = vertices[i].z;
		float y2 = vertices[i + 3].z;
		// line 2
		float x3 = vertices[i + 2].x;
		float x4 = vertices[i + 4].x;
		float y3 = vertices[i + 2].z;
		float y4 = vertices[i + 4].z;

		// calc line 1
		float m1 = (y2 - y1)/(x2 - x1);
		float c1 = y1 - m1 * x1;

		// calc line 2
		float m2 = (y4 - y3) / (x4 - x3);
		float c2 = y3 - m2 * x3;

		//find intersection of lines
		float interX = (c2 - c1) / (m1 - m2);
		float interY = m2 * interX + c2;

		vertices.push_back(glm::vec3(interX, 2.0f, interY));
		vDown.push_back(glm::vec3(interX, 0.0f, interY));
	}
	vertices.insert(vertices.end(), vDown.begin(), vDown.end());

	for (int i = 0; i < sides; ++i) {
		int lc0 = i;
		int lc1 = (i + 2) % sides;
		int hc0 = lc0 + sides;
		int hc1 = lc1 + sides;

		faces.push_back(Face(lc0, lc1, i + 2 * sides));
		faces.push_back(Face(hc0, hc1, i + 3 * sides));
	}

	return MeshModel(faces, vertices, Utils::CalculateNormals(vertices, faces), std::vector<glm::vec2>(), "Uniform Prism");
}

PrimMeshModel::~PrimMeshModel()
{
}
