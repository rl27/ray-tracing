#ifndef BVH_H
#define BVH_H

#include <vector>

class BVH
{
public:
	BVH(std::vector<Triangle*> &t, int d) : hasLeaf(false), depth(d), triangles(t)
	{
		for (int i = 0; i < triangles.size(); i++)
			box.expand(triangles.at(i)->box.bounds);

		center = box.getCenter();
		longest = box.getMaxDimension();
		dimensions.e[0] = box.bounds[1] - box.bounds[0];
		dimensions.e[1] = box.bounds[3] - box.bounds[2];
		dimensions.e[2] = box.bounds[5] - box.bounds[4];
		dimensions.e[longest] *= 0.5;

		this->divide();
	}

	void hit(const Ray &r, std::vector<Triangle*> &t);
	void divide();
	bool contains(const Vec &center, const Vec &dim, const Vec &ve);

	int depth;
	bool hasLeaf;
	std::vector<Triangle*> triangles;
	BVH* child[2];
	Box box;
	Vec dimensions;
	Vec center;
	int longest;
};

void BVH::divide()
{
	if (triangles.size() > 4 && depth < 10)
	{
		std::vector<Triangle*> temp;

		int neg[2] = { -1,1 };
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < triangles.size(); j++)
			{
				Vec t(0, 0, 0);
				t.e[longest] = dimensions.e[longest] * 0.5;
				if (contains(center + (t * neg[i]), dimensions, triangles.at(j)->center))
					temp.push_back(triangles.at(j));
			}

			if (temp.size() > 0)
				child[i] = new BVH(temp, depth + 1);
			else
				child[i] = NULL;

			temp.clear();
		}

		hasLeaf = true;
	}
}

void BVH::hit(const Ray &r, std::vector<Triangle*> &t)
{
	if (box.hitBox(r))
	{
		if (hasLeaf)
		{
			if (child[0])
				child[0]->hit(r, t);

			if (child[1])
				child[1]->hit(r, t);
		}
		else
		{
			for (int i = 0; i < triangles.size(); i++)
				t.push_back(triangles.at(i));
		}
	}
}

bool BVH::contains(const Vec &center, const Vec &dim, const Vec &ve)
{
	if (ve.e[0] >= center.e[0] + dim.e[0] / 2 || ve.e[0] < center.e[0] - dim.e[0] / 2) return false;
	if (ve.e[1] >= center.e[1] + dim.e[1] / 2 || ve.e[1] < center.e[1] - dim.e[1] / 2) return false;
	if (ve.e[2] >= center.e[2] + dim.e[2] / 2 || ve.e[2] < center.e[2] - dim.e[2] / 2) return false;
	return true;
}

#endif