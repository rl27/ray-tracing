#ifndef OCTREE_H
#define OCTREE_H

#include "Mesh.h"

class Octree
{
public:
	Octree(Vec cen, float len, int d, std::vector<Mesh*> &m) : center(cen), length(len), hasLeaf(false), depth(d), meshes(m)
	{
		// DIVIDE THE OCTREE FURTHER IF NEEDED
		this->divide();

		// INITIALIZE THE BOX FOR ALL CONTAINED MESHES
		for (int i = 0; i < meshes.size(); i++)
			box.expand(meshes.at(i)->box.bounds);
	}

	// Does a cube contain the center of a mesh?
	static bool contains(const Vec &center, const float &le, const Vec &ve);
	// Do we need to divide each of the children?
	void dispense();
	void divide();
	void hit(const Ray &r, std::vector<Mesh*> &m);

	int depth;
	bool hasLeaf;
	std::vector<Mesh*> meshes;
	Octree* child[8];
	Box box;
	Vec center;
	float length;
};

void Octree::hit(const Ray &r, std::vector<Mesh*> &m)
{
	// Check if the current box is hit
	if (box.hitBox(r))
	{
		// If the current box has children, call hit
		if (hasLeaf)
		{
			for (int i = 0; i < 8; i++)
			{
				if (child[i]->meshes.size() > 0)
				{
					child[i]->hit(r, m);
				}
			}
		}
		// The current node has no children; take the mesh
		else
		{
			m.push_back(meshes.at(0));
		}
	}
}

/*
bool Octree::hit(const Ray &r, std::vector<Mesh*> &m)
{
	// Check if the current box is hit
	if (box.hitBox(r))
	{
		// If the current box has children, call hit
		if (hasLeaf)
		{
			for (int i = 0; i < 8; i++)
			{
				if (child[i]->meshes.size() > 0)
				{
					if (child[i]->hit(r, m)) return true;
				}
			}
		}
		// The current node has no children; take the mesh
		else
		{
			m.push_back(meshes.at(0));
			return true;
		}
	}
	return false;
}
*/

bool Octree::contains(const Vec &center, const float &le, const Vec &ve)
{
	if (ve.e[0] > center.e[0] + le / 2 || ve.e[0] < center.e[0] - le / 2) return false;
	if (ve.e[1] > center.e[1] + le / 2 || ve.e[1] < center.e[1] - le / 2) return false;
	if (ve.e[2] > center.e[2] + le / 2 || ve.e[2] < center.e[2] - le / 2) return false;
	return true;
}

void Octree::dispense()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < meshes.size(); j++)
		{
			if (child[i]->contains(Vec(0, 0, 0), 5, meshes.at(j)->center))
				child[i]->meshes.push_back(meshes.at(j));
		}
	}
}

void Octree::divide()
{
	if (meshes.size() >= 2 && depth < 3)
	{
		std::vector<Mesh*> temp;

		int neg[24] = { 1,1,1,1,1,-1,1,-1,1,1,-1,-1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1 };
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < meshes.size(); j++)
			{
				if (contains(center + Vec((length / 4)*neg[i * 3], (length / 4)*neg[i * 3 + 1], (length / 4)*neg[i * 3 + 2]), length / 2, meshes.at(j)->center))
					temp.push_back(meshes.at(j));
			}
			child[i] = new Octree(center + Vec((length / 4)*neg[i * 3], (length / 4)*neg[i * 3 + 1], (length / 4)*neg[i * 3 + 2]), length / 2, depth + 1, temp);
			temp.clear();
		}

		/*
		// INITIALIZE THE CHILDREN
		child[0] = new Octree(center + Vec( length / 4,  length / 4,  length / 4), length / 2, depth + 1);
		child[1] = new Octree(center + Vec( length / 4,  length / 4, -length / 4), length / 2, depth + 1);
		child[2] = new Octree(center + Vec( length / 4, -length / 4,  length / 4), length / 2, depth + 1);
		child[3] = new Octree(center + Vec( length / 4, -length / 4, -length / 4), length / 2, depth + 1);
		child[4] = new Octree(center + Vec(-length / 4,  length / 4,  length / 4), length / 2, depth + 1);
		child[5] = new Octree(center + Vec(-length / 4,  length / 4, -length / 4), length / 2, depth + 1);
		child[6] = new Octree(center + Vec(-length / 4, -length / 4,  length / 4), length / 2, depth + 1);
		child[7] = new Octree(center + Vec(-length / 4, -length / 4, -length / 4), length / 2, depth + 1);

		this->dispense();
		*/
		hasLeaf = true;
	}
}

#endif