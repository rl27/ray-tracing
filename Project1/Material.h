#ifndef MATERIAL_H
#define MATERIAL_H

#include "Object.h"

Vec unit_sphere()
{
	Vec p;
	do { p = Vec(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0))*2.0 - Vec(1, 1, 1); }
	while (p.squared_length() >= 1.0);
	return p;
}

Vec reflect(const Vec &v, const Vec &n) { return v - n*(v.dot(n))*2; }

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

class Material
{
public:
	// Take a Ray and its hit data; give a new Ray and its attenuation.
	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const = 0;
};

// Diffuse material. Either:
// 1. Always scatter and attenuate by the reflection (Done here), or
// 2. Scatter with no attenuation but absorb a fraction of the Rays. Scatter prob. p, attenuation = albedo/p.
class Lambertian : public Material
{
public:
	Lambertian(const Vec &a) { albedo = a; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
	{
		Vec target = rec.p + rec.normal + unit_sphere();
		scatt = Ray(rec.p, target - rec.p);
		att = albedo;
		return true;
	}

	Vec albedo;
};

// Always reflects the Ray, can make it "fuzzy" by reflecting to within an area. Fuzz 0 = mirror.
class Metal : public Material
{
public:
	Metal(const Vec &a, float f) { albedo = a; if (f < 1) fuzz = f; else fuzz = f; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
	{
		Vec ref = reflect((r.direction())/*.unit_vec()*/, rec.normal);
		scatt = Ray(rec.p, ref + unit_sphere()*fuzz);
		att = albedo;
		return ((scatt.direction()).dot(rec.normal) > 0);
	}

	Vec albedo;
	float fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(float ri) { ref_idx = ri; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
	{
		Vec out_normal;
		Vec reflected = reflect(r.direction(), rec.normal);
		Vec refracted;
		float ni_over_nt;
		att = Vec(1.0, 1.0, 1.0); // No absorbtion
		float cos;
		if ((r.direction()).dot(rec.normal) > 0) // if it = 0, then the direction is perpendicular to the surface.
		{
			out_normal = Vec(-rec.normal.e[0], -rec.normal.e[1], -rec.normal.e[2]);
			ni_over_nt = ref_idx;
			cos = ref_idx * (r.direction()).dot(rec.normal) / (r.direction()).length();
		}
		else
		{
			out_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cos = -(r.direction()).dot(rec.normal) / (r.direction()).length();
		}
		if (refract(r.direction(), out_normal, ni_over_nt, refracted) &&
		    schlick(cos, ref_idx) < (rand() / (RAND_MAX + 1.0)))
			scatt = Ray(rec.p, refracted);
		else
			scatt = Ray(rec.p, reflected);

		return true;
	}

	float ref_idx;
};

class Barycentric : public Material
{
public:
	Barycentric(const float &uin, const float &vin) { u = uin; v = vin; }
	Barycentric() {}
	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
	{
		Vec target = rec.p + rec.normal + unit_sphere();
		//Vec target = reflect(r.direction(), rec.normal);
		scatt = Ray(rec.p, target - rec.p);
		//att = Vec(u, v, 1 - u - v);
		att = Vec(0.7, 0.7, 0);
		return true;
	}

	float u;
	float v;
};

#endif