#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(Vec c, float r, Material *m) : center(c), radius(r), mat(m)
	{
		box.bounds[0] = center.e[0] - radius;
		box.bounds[1] = center.e[0] + radius;
		box.bounds[2] = center.e[1] - radius;
		box.bounds[3] = center.e[1] + radius;
		box.bounds[4] = center.e[2] - radius;
		box.bounds[5] = center.e[2] + radius;
		box.center = center;
	}

	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);
	virtual Box getBox();

	Box box;
	Vec center;
	float radius;
	Material *mat;
};

#endif
