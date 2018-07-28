#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(Vec c, float r, Material *m) : center(c), radius(r), mat(m) {}

	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	Vec center;
	float radius;
	Material *mat;
};

bool Sphere::hit(const Ray &r, float t_min, float t_max, hit_record &rec)
{
	Vec oc = r.origin() - center;
	float a = (r.direction()).dot(r.direction());
	float b = 2 * oc.dot(r.direction());
	float c = oc.dot(oc) - radius * radius;
	float disc = b * b - 4 * a*c;

	if (disc > 0)
	{
		float temp = (-b - sqrt(disc)) / (2 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.get_point_at(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
		temp = (-b + sqrt(disc)) / (2 * a);
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.get_point_at(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
	}
	return false;
}

#endif