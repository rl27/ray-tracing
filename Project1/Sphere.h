#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(Vec c, float r, Material *m) : center(c), radius(r), mat(m) {}

	// Implementation below
	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	Vec center;
	float radius;
	Material *mat;
};

// Check to see if the Sphere is hit by a Ray. If so, record it.
bool Sphere::hit(const Ray &r, float t_min, float t_max, hit_record &rec)
{
	// Use: (p(t)-C)•(p(t)-C)=R^2. This becomes ((A-C)+t*B)•((A-C)+t*B). Expand and solve for t.
	// Quadratic equation for t: t^2(B•B)+2t(B•(A-C))+[(A-C)•(A-C)-R^2]=0
	Vec oc = r.origin() - center;
	float a = (r.direction()).dot(r.direction());
	float b = 2 * oc.dot(r.direction());
	float c = oc.dot(oc) - radius*radius;
	float disc = b * b - 4 * a*c;

	if (disc > 0)
	{
		// If one solution for t does not work (in this case, it needs to be positive), try the other solution.
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
	// Neither solution for t worked; there is no hit.
	return false;
}

#endif