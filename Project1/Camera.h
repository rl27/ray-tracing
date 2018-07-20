#ifndef CAMERA_H
#define CAMERA_H

#define _USE_MATH_DEFINES // M_PI constant
#include <math.h>
#include "Ray.h"

Vec unit_circle()
{
	Vec p;
	do { p = Vec((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), 0) - Vec(1, 1, 0); }
	while (p.dot(p) >= 1.0);
	return p;
}

class Camera
{
public:
	Camera(Vec from, Vec to, Vec vup, float vfov, float aspect, float aperture, float focus_dist)
	{
		lens_radius = aperture / 2;
		float theta = vfov * M_PI / 180;  // Convert degrees to radians
		float half_height = tan(theta / 2);  // Get half height and width based on the vertical Field Of View and the aspect ratio
		float half_width = aspect * half_height;

		w = (from - to).unit_vec();
		u = (vup.cross(w)).unit_vec();
		v = w.cross(u);

		lower_left = from - u * focus_dist*half_width - v * focus_dist*half_height - w * focus_dist;
		x_comp = u * 2 * focus_dist*half_width;
		y_comp = v * 2 * focus_dist*half_height;
		origin = from;
	}

	Ray get_ray(float s, float t)
	{
		// Let the Ray begin from anywhere within a circle around the origin
		// This simulates a thin lens
		Vec rd = unit_circle() * lens_radius;
		Vec offset = u * rd.e[0] + v * rd.e[1];
		return Ray(origin + offset, lower_left + x_comp * s + y_comp * t - origin - offset);
	}

	Vec lower_left;
	Vec x_comp;
	Vec y_comp;
	Vec origin;
	Vec u, v, w; // Unit vectors for horizontal, vertical, etc. directions.
	float lens_radius; // Defocus blur
};

#endif