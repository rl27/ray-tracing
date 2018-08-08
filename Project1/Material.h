#ifndef MATERIAL_H
#define MATERIAL_H

#include "Object.h"
#include "Utility.h"

class Material
{
public:
	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const = 0;
	Vec emittance;
	bool isDiffuse = false;
};

class Lambertian : public Material
{
public:
	Lambertian(const Vec &a, const Vec &e = Vec(0, 0, 0)) { albedo = a; emittance = e; isDiffuse = true; }

	virtual bool scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const;
	Vec albedo;
};

class Metal : public Material
{
public:
	Metal(const Vec &a, float f, const Vec &e = Vec(0, 0, 0)) { albedo = a; if (f < 1) fuzz = f; else fuzz = 1; emittance = e; }

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

#endif