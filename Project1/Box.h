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
		center = Vec();
	}
	void expand(const float &a, const float &b, const float &c);
	void expand(float b[6]);
	bool hitBox(const Ray &r);
	Vec getCenter();
	float getMaxLength();
	int getMaxDimension();

	float bounds[6];
	Vec center;
};


#endif