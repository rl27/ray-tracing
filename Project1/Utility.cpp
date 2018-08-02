#define _USE_MATH_DEFINES
#include <math.h>
#include "Utility.h"

Vec random_on_sphere(const Vec &c, const float &r)
{
	double theta = (rand() / (RAND_MAX + 1.0)) * M_PI;
	double phi = (rand() / (RAND_MAX + 1.0)) * 2 * M_PI;
	double dxr = r * sin(theta) * cos(phi);
	double dyr = r * sin(theta) * sin(phi);
	double dzr = r * cos(theta);
	return Vec(c.e[0] + dxr, c.e[1] + dyr, c.e[2] + dzr);
}

Vec unit_circle()
{
	Vec p;
	do { p = Vec((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), 0) - Vec(1, 1, 0); } while (p.dot(p) > 1.0);
	return p;
}

Vec unit_sphere()
{
	Vec p;
	do { p = Vec(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0))*2.0 - Vec(1, 1, 1); } while (p.squared_length() > 1.0);
	return p;
}

Vec hemisphere(Vec n)
{
	Vec p;
	do { p = unit_sphere(); } while (p.dot(n) < 0);
	return p;
}

Vec reflect(const Vec &v, const Vec &n) { return v - n * (v.dot(n)) * 2; }

bool refract(const Vec &v, const Vec &n, float ni_over_nt, Vec &refracted)
{
	Vec uv = v.unit_vec();
	float dt = uv.dot(n);
	float disc = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
	if (disc > 0)
	{
		refracted = (uv - n * dt) * ni_over_nt - n * sqrt(disc);
		return true;
	}
	return false;
}

float schlick(float cos, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cos), 5);
}

float clamp(float x)
{
	if (x < 0) return 0;
	if (x > 1) return 1;
	return x;
}