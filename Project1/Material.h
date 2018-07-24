#ifndef MATERIAL_H
#define MATERIAL_H

#include "Object.h"
#include "Utility.h"

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

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const;
	Vec albedo;
};

// Always reflects the Ray, can make it "fuzzy" by reflecting to within an area. Fuzz 0 = mirror.
class Metal : public Material
{
public:
	Metal(const Vec &a, float f) { albedo = a; if (f < 1) fuzz = f; else fuzz = f; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const;

	Vec albedo;
	float fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(float ri) { ref_idx = ri; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const;

	float ref_idx;
};

class Barycentric : public Material
{
public:
	Barycentric(const float &uin, const float &vin) { u = uin; v = vin; }
	Barycentric() {}
	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const;

	float u;
	float v;
};

#endif