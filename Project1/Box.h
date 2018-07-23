#ifndef BOX_H
#define BOX_H

#include "Ray.h"

class Box
{
public:
	Box()
	{
		bounds[0] = 1e5;
		bounds[1] = -1e5;
		bounds[2] = 1e5;
		bounds[3] = -1e5;
		bounds[4] = 1e5;
		bounds[5] = -1e5;
	}
	void expand(const float &a, const float &b, const float &c);
	void expand(float b[6]);
	bool hitBox(const Ray &r);
	Vec getCenter();
	float getMaxLength();

	float bounds[6];
};

Vec Box::getCenter()
{
	return (Vec(bounds[0], bounds[2], bounds[4]) + Vec(bounds[1], bounds[3], bounds[5])) / 2;
}

float Box::getMaxLength()
{
	float a = bounds[1] - bounds[0];
	float b = bounds[3] - bounds[2];
	float c = bounds[5] - bounds[4];
	if (a > b) return (a > c ? a : c);
	else return (b > c ? b : c);
}

void Box::expand(const float &a, const float &b, const float &c)
{
	if (a < bounds[0]) bounds[0] = a;
	if (a > bounds[1]) bounds[1] = a;
	if (b < bounds[2]) bounds[2] = b;
	if (b > bounds[3]) bounds[3] = b;
	if (c < bounds[4]) bounds[4] = c;
	if (c > bounds[5]) bounds[5] = c;
}

void Box::expand(float b[6])
{
	this->expand(b[0], b[2], b[4]);
	this->expand(b[1], b[3], b[5]);
}

bool Box::hitBox(const Ray &r)
{
	float xmin, xmax, ymin, ymax, zmin, zmax;

	xmin = (bounds[r.sign[0]] - r.A.e[0]) * r.inv.e[0];
	xmax = (bounds[1 - r.sign[0]] - r.A.e[0]) * r.inv.e[0];
	ymin = (bounds[2 + r.sign[1]] - r.A.e[1]) * r.inv.e[1];
	ymax = (bounds[3 - r.sign[1]] - r.A.e[1]) * r.inv.e[1];

	if ((xmin > ymax) || (ymin > xmax))
		return false;
	if (ymin > xmin)
		xmin = ymin;
	if (ymax < xmax)
		xmax = ymax;

	zmin = (bounds[4 + r.sign[2]] - r.A.e[2]) * r.inv.e[2];
	zmax = (bounds[5 - r.sign[2]] - r.A.e[2]) * r.inv.e[2];

	if ((xmin > zmax) || (zmin > xmax))
		return false;
	if (zmin > xmin)
		xmin = zmin;
	if (zmax < xmax)
		xmax = zmax;

	return true;
}

#endif