#ifndef MESH_H
#define MESH_H

#include "Triangle.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh
{
public:
	Mesh(const std::string &file, const Vec &off);

	bool importFile(const std::string &file);
	bool getMeshData(const aiMesh *mesh);
	bool processData(const aiScene* scene);

	std::vector<Vec> vertices;
	std::vector<int> indices;
	std::vector<Triangle*> triangles;
	Vec offset;
};



#endif