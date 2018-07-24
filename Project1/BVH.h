#ifndef BVH_H
#define BVH_H

#include <vector>
#include "Triangle.h"

class BVH
{
public:
	BVH(std::vector<Triangle*> &t, int d);

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


#endif