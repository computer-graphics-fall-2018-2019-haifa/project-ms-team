#pragma once
#include "MeshModel.h"

class PrimMeshModel
{
private:
	PrimMeshModel();
public:
	static MeshModel genCube();
	static MeshModel genPyramid();
	~PrimMeshModel();
};

