#ifndef BOX_H
#define BOX_H

#include "Ray.h"

class Box
{
public:
	Box()
	{
		/*
		planes[0] = Vec(1, 0, 0);
		planes[1] = Vec(0, 1, 0);
		planes[2] = Vec(0, 0, 1);
		planes[3] = Vec( sqrtf(3) / 3,  sqrtf(3) / 3, sqrtf(3) / 3);
		planes[4] = Vec(-sqrtf(3) / 3,  sqrtf(3) / 3, sqrtf(3) / 3);
		planes[5] = Vec(-sqrtf(3) / 3, -sqrtf(3) / 3, sqrtf(3) / 3);
		planes[6] = Vec( sqrtf(3) / 3, -sqrtf(3) / 3, sqrtf(3) / 3);
		*/

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
	//float extents[7][2];
	//Vec planes[7];
};

Vec Box::getCenter()
{
	return Vec(bounds[0], bounds[1], bounds[2]) + Vec(bounds[3], bounds[4], bounds[5]) / 2;
}

float Box::getMaxLength()
{
	Vec center = this->getCenter();
	float a = bounds[1] - center.e[0];
	float b = bounds[3] - center.e[1];
	float c = bounds[5] - center.e[1];
	if (a > b)
	{
		if (a > c) return a * 2;
		else return c * 2;
	}
	else
	{
		if (b > c) return b * 2;
		else return c * 2;
	}
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
	float a = b[0];
	float c = b[2];
	float d = b[4];
	this->expand(a, c, d);
	a = b[1];
	c = b[3];
	d = b[5];
	this->expand(a, c, d);
}

bool Box::hitBox(const Ray &r)
{
	float xmin, xmax, ymin, ymax, zmin, zmax;

	xmin = (bounds[r.sign[0]] - r.A.e[0]) * r.inv.e[0];
	xmax = (bounds[1 - r.sign[0]] - r.A.e[0]) * r.inv.e[0];
	ymin = (bounds[r.sign[1]] - r.A.e[1]) * r.inv.e[1];
	ymax = (bounds[1 - r.sign[1]] - r.A.e[1]) * r.inv.e[1];

	if ((xmin > ymax) || (ymin > xmax))
		return false;
	if (ymin > xmin)
		xmin = ymin;
	if (ymax < xmax)
		xmax = ymax;

	zmin = (bounds[r.sign[2]] - r.A.e[2]) * r.inv.e[2];
	zmax = (bounds[1 - r.sign[2]] - r.A.e[2]) * r.inv.e[2];

	if ((xmin > zmax) || (zmin > xmax))
		return false;
	if (zmin > xmin)
		xmin = zmin;
	if (zmax < xmax)
		xmax = zmax;

	return true;
}

#endif