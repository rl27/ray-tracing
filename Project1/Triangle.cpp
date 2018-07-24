#include "Triangle.h"

Triangle::Triangle(Vec a, Vec b, Vec c, Material *m) : v0(a), v1(b), v2(c), mat(m)
{
	N = (v1 - v0).cross(v2 - v0);
	box.expand(v0.e[0], v0.e[1], v0.e[2]);
	box.expand(v1.e[0], v1.e[1], v1.e[2]);
	box.expand(v2.e[0], v2.e[1], v2.e[2]);
	center = box.getCenter();
}

bool Triangle::hit(const Ray &r, float tmin, float tmax, hit_record &rec)
{
	if (!box.hitBox(r)) return false;

	Vec e0 = v0 - v2;
	Vec e1 = v1 - v2;

	Vec pvec = (r.direction()).cross(e1);
	float det = (pvec).dot(e0);

	if ((det) < 1e-4) return false;

	float invDet = 1 / det;

	Vec tvec = r.origin() - v2;
	float temp = tvec.dot(pvec) * invDet;
	if (temp < 0 || temp > 1) return false;

	Vec qvec = tvec.cross(e0);
	float temp2 = (r.direction()).dot(qvec) * invDet;
	if (temp2 < 0 || temp + temp2 > 1) return false;

	float t = e1.dot(qvec) * invDet;
	if (t < tmin || t > tmax) return false;

	rec.t = t;
	rec.p = r.get_point_at(t);
	rec.normal = N.unit_vec();
	rec.mat = mat;

	return true;
}