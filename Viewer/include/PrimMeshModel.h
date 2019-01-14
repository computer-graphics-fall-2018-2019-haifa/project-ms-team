#pragma once
#include <iostream>
#include "MeshModel.h"
#include "Utils.h"


class PrimMeshModel
{
private:
	PrimMeshModel();
public:
	static MeshModel genCube();
	static MeshModel genPyramid();
	static MeshModel genUniform(int sides);
	~PrimMeshModel();
};

