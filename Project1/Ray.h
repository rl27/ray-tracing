#ifndef RAY_H
#define RAY_H

#include "Vec.h"

class Ray
{
public:
    // Initialize the ray with two vectors.
	Ray() { A = Vec(0.0, 0.0, 0.0); B = Vec(0.0, 0.0, 0.0); }
	Ray(const Vec &a, const Vec &b)
	{
		A = a;
		B = b;
		inv = Vec(1 / B.e[0], 1 / B.e[1], 1 / B.e[2]);
		sign[0] = (inv.e[0] < 0);
		sign[1] = (inv.e[1] < 0);
		sign[2] = (inv.e[2] < 0);
	}

    // Functions
    Vec origin()    const { return A; }
    Vec direction() const { return B; }
    Vec get_point_at(float t) const { return A + B*t; }

    // Origin and direction
    Vec A;
    Vec B;
	Vec inv;
	int sign[3];
};

#endif
