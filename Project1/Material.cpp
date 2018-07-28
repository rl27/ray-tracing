#include "Material.h"

Vec Lambertian::gete()
{
	return emittance;
}

Vec Metal::gete()
{
	return emittance;
}

Vec Dielectric::gete()
{
	return emittance;
}

bool Lambertian::scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
{
	Vec target = rec.p + hemisphere(rec.normal);
	scatt = Ray(rec.p, target - rec.p);
	att = albedo;
	return true;
}

bool Metal::scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
{
	Vec ref = reflect((r.direction())/*.unit_vec()*/, rec.normal);
	scatt = Ray(rec.p, ref + unit_sphere()*fuzz);
	att = albedo;
	return ((scatt.direction()).dot(rec.normal) > 0);
}

bool Dielectric::scatter(const Ray &r, const hit_record &rec, Vec &att, Ray &scatt) const
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