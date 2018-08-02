#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Material.h"

class Triangle : public Object
{
public:
	Triangle(Vec a, Vec b, Vec c, Material *m);
	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	Vec v0;
	Vec v1;
	Vec v2;
	Vec N;
	Vec center;
};



#endif