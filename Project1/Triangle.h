#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include "Material.h"

class Triangle : public Object
{
public:
	Triangle(Vec a, Vec b, Vec c, Material *m) : v0(a), v1(b), v2(c), mat(m) { N = (v1 - v0).cross(v2 - v0); }

	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	mutable Vec v0;
	mutable Vec v1;
	mutable Vec v2;
	Vec N;
	Material *mat;
};

bool Triangle::hit(const Ray &r, float tmin, float tmax, hit_record &rec)
{
	// MOLLER-TRUMBORE ALGORITHM
	Vec e0 = v0 - v2;
	Vec e1 = v1 - v2;

	Vec pvec = (r.direction()).cross(e1);
	float det = (pvec).dot(e0);

	if (fabs(det) < 1e-4) return false;

	float invDet = 1 / det;

	Vec tvec = r.origin() - v2;
	float temp = tvec.dot(pvec) * invDet;
	if (temp < 0 || temp > 1) return false;

	Vec qvec = tvec.cross(e0);
	float temp2 = (r.direction()).dot(qvec) * invDet;
	if (temp2 < 0 || temp + temp2 > 1) return false;

	float t = e1.dot(qvec) * invDet;
	if (t < tmin || t > tmax) return false;

	/*
	// CONVENTIONAL METHOD
	// Is the Ray parallel to the triangle/plane?
	float D = N.dot(r.direction());
	if (fabs(D) < 1e-3)
		return false;

	float t = (N.dot(v0) - N.dot(r.origin())) / D;

	if (t < 1e-3)
		return false;

	Vec P = r.get_point_at(t); // point where Ray hits the plane
	Vec C; // vector perpendicular to triangle's plane

	// Does the Ray hit the triangle? Test each edge
	C = (v1 - v0).cross(P - v0);
	if (N.dot(C) < 0) return false;

	C = (v2 - v1).cross(P - v1);
	if (N.dot(C) < 0) return false;
	//mat->u = C.length() / N.length();
	mat->u = N.dot(C) / N.dot(N);

	C = (v0 - v2).cross(P - v2);
	if (N.dot(C) < 0) return false;
	//mat->v = C.length() / N.length();
	mat->v = N.dot(C) / N.dot(N);
	
	//float denom = N.dot(N);
	//mat->u /= denom;
	//mat->v /= denom;
	*/

	rec.t = t;
	rec.p = r.get_point_at(t);
	rec.normal = N.unit_vec();
	rec.mat = mat;

	return true;
}

#endif