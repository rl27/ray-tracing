#ifndef OCTREE_H
#define OCTREE_H

#include "Mesh.h"
#include <iostream>
class Octree
{
public:
	Octree(std::vector<Triangle*> &t, int d) : hasLeaf(false), depth(d), triangles(t)
	{
		for (int i = 0; i < triangles.size(); i++)
			box.expand(triangles.at(i)->box.bounds);

		center = box.getCenter();
		length = box.getMaxLength();

		this->divide();
	}

	static bool contains(const Vec &center, const float &le, const Vec &ve);
	void divide();
	void hit(const Ray &r, std::vector<Triangle*> &t);

	int depth;
	bool hasLeaf;
	std::vector<Triangle*> triangles;
	Octree* child[8];
	Box box;
	Vec center;
	float length;
};

void Octree::hit(const Ray &r, std::vector<Triangle*> &t)
{
	if (box.hitBox(r))
	{
		if (hasLeaf)
		{
			for (int i = 0; i < 8; i++)
			{
				if (child[i])
					child[i]->hit(r, t);
			}
		}
		else
		{
			for (int i = 0; i < triangles.size(); i++)
				t.push_back(triangles.at(i));
		}
	}
}

bool Octree::contains(const Vec &center, const float &le, const Vec &ve)
{

	if (ve.e[0] >= center.e[0] + le / 2 || ve.e[0] < center.e[0] - le / 2) return false;
	if (ve.e[1] >= center.e[1] + le / 2 || ve.e[1] < center.e[1] - le / 2) return false;
	if (ve.e[2] >= center.e[2] + le / 2 || ve.e[2] < center.e[2] - le / 2) return false;
	return true;
}

void Octree::divide()
{
	if (triangles.size() > 4 && depth < 4)
	{
		std::vector<Triangle*> temp;

		int neg[24] = { 1,1,1,1,1,-1,1,-1,1,1,-1,-1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1 };
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < triangles.size(); j++)
			{
				if (contains(center + Vec((length / 4)*neg[i * 3], (length / 4)*neg[i * 3 + 1], (length / 4)*neg[i * 3 + 2]), length / 2, triangles.at(j)->center))
					temp.push_back(triangles.at(j));
			}

			if (temp.size() > 0)
				child[i] = new Octree(temp, depth + 1);
			else
				child[i] = NULL;

			temp.clear();
		}

		hasLeaf = true;
	}
}

#endif