#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include "Triangle.h"

class Octree
{
public:

	Octree(std::vector<Object*> &t, int d);
	static bool contains(const Vec &center, const float &le, const Vec &ve);
	void divide();
	void hit(const Ray &r, std::vector<Object*> *t);

	int depth;
	bool hasLeaf;
	std::vector<Object*> triangles;
	Octree* child[8];
	Box box;
	Vec center;
	float length;
};



#endif