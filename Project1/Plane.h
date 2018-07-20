#ifndef PLANE_H
#define PLANE_H

#include "Object.h"

class Plane : public Object
{
public:
	Plane(Vec a, Vec norm, Material *m) : normal(norm), point(a), mat(m) {}

	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);
	Vec point;
	Vec normal;
	Material *mat;
};

bool Plane::hit(const Ray &r, float t_min, float t_max, hit_record &rec)
{
	float n = normal.dot(r.direction());
	if (fabs(n) < 1e-3)
		return false;

	float temp = (normal.dot(point) - normal.dot(r.origin())) / n;

	if (temp > t_max || temp < t_min)
		return false;

	/*
	if((r.get_point_at(temp) - point).length() > radius)
		return false;
	*/

	rec.t = temp;
	rec.p = r.get_point_at(rec.t);
	rec.normal = normal.unit_vec();
	rec.mat = mat;
	return true;
}

#endif
