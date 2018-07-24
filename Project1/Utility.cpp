#include "Utility.h"

Vec unit_circle()
{
	Vec p;
	do { p = Vec((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), 0) - Vec(1, 1, 0); } while (p.dot(p) >= 1.0);
	return p;
}

Vec unit_sphere()
{
	Vec p;
	do { p = Vec(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0))*2.0 - Vec(1, 1, 1); } while (p.squared_length() >= 1.0);
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
