#define _USE_MATH_DEFINES
#include <math.h>
#include "Sphere.h"

Vec Sphere::random_on_surface()
{
	double theta = (rand() / (RAND_MAX + 1.0)) * M_PI;
	double phi = (rand() / (RAND_MAX + 1.0)) * 2 * M_PI;
	double dxr = radius * sin(theta) * cos(phi);
	double dyr = radius * sin(theta) * sin(phi);
	double dzr = radius * cos(theta);
	return Vec(center.e[0] + dxr, center.e[1] + dyr, center.e[2] + dzr);
}

bool Sphere::hit(const Ray &r, float t_min, float t_max, hit_record &rec)
{
	if (!box.hitBox(r)) return false;

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