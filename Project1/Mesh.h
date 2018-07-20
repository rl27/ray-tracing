#ifndef MESH_H
#define MESH_H

#include "Object.h"
#include "Box.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public Object
{
public:
	Mesh(const std::string &file, const Vec &off) : offset(off)
	{
		importFile(file);
		for (int i = 0; i < indices.size() / 3; i++)
			triangles.push_back(Triangle(vertices.at(indices.at(3 * i)), vertices.at(indices.at(3 * i + 1)), vertices.at(indices.at(3 * i + 2)),
				new Metal(Vec(0.75, 0.75, 0.75), 0)));
		center = box.getCenter();
	}

	bool importFile(const std::string &file);
	bool getMeshData(const aiMesh *mesh);
	bool processData(const aiScene* scene);

	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	std::vector<Vec> vertices;
	std::vector<int> indices;
	std::vector<Triangle> triangles;
	Box box;
	Vec center;
	Vec offset;
};

bool Mesh::importFile(const std::string &file)
{
	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile(file, /*aiProcessPreset_TargetRealtime_Quality |*/ aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices/* | aiProcess_GenNormals | aiProcess_FixInfacingNormals*/);
	if (!scene) return false;
	else processData(scene);

	return true;
}

bool Mesh::getMeshData(const aiMesh *mesh)
{
	aiFace *face;

	for (unsigned int v = 0; v < mesh->mNumVertices; v++)
	{
		vertices.push_back(Vec(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
		box.expand(vertices.at(v).e[0], vertices.at(v).e[1], vertices.at(v).e[2]);

		/*
		vertices.push_back(mesh->mNormals[v].x);
		vertices.push_back(mesh->mNormals[v].y);
		vertices.push_back(mesh->mNormals[v].z);

		if (mesh->HasTextureCoords(0))
		{
			textures.push_back(mesh->mTextureCoords[0][v].x);
			textures.push_back(mesh->mTextureCoords[0][v].y);
		}
		else
		{
			textures.push_back(0);
			textures.push_back(0);
		}

		vertices.push_back(mesh->mTangents[v].x);
		vertices.push_back(mesh->mTangents[v].y);
		vertices.push_back(mesh->mTangents[v].z);
		*/
	}

	for (unsigned int f = 0; f<mesh->mNumFaces; f++)
	{
		face = &mesh->mFaces[f];
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	return true;
}

bool Mesh::processData(const aiScene* scene)
{
	bool repeat = true;

	std::vector<const aiNode*> nodeBuff;
	const aiNode* modelNode;

	nodeBuff.push_back(scene->mRootNode);


	/* if (modelScene->mNumMeshes > 0)
	{
	for (unsigned int m=0;m<modelScene->mNumMeshes;m++)
	this->assimpGetMeshData(modelScene->mMeshes[m]);
	}*/

	while (repeat)
	{
		for (int a = 0; a < nodeBuff.size(); a++)
		{
			modelNode = nodeBuff.at(a);
			if (modelNode->mNumChildren > 0)
				for (unsigned int c = 0; c < modelNode->mNumChildren; c++)
					nodeBuff.push_back(modelNode->mChildren[c]);

			else repeat = false;
		}
	}

	for (int a = 0; a < nodeBuff.size(); a++)
	{
		modelNode = nodeBuff.at(a);

		if (modelNode->mNumMeshes > 0)
		{
			for (int b = 0; b < modelNode->mNumMeshes; b++)
				getMeshData(scene->mMeshes[b]);
		}
	}

	return true;
}

bool Mesh::hit(const Ray &r, float tmin, float tmax, hit_record &rec)
{
	hit_record temp;
	bool hit = false;
	float tempmax = tmax;

	if (!box.hitBox(r)) return false;

	for (int i = 0; i < triangles.size(); i++)
	{
		if ((triangles.at(i)).hit(r, tmin, tempmax, temp))
		{
			hit = true;
			tempmax = temp.t;
			rec = temp;
		}
	}

	return hit;
}

#endif