#ifndef CAMERA_H
#define CAMERA_H

#define _USE_MATH_DEFINES // M_PI constant
#include <math.h>
#include "Ray.h"
#include "Utility.h"

class Camera
{
public:
	Camera(Vec from, Vec to, Vec vup, float vfov, float aspect, float aperture, float focus_dist);

	Ray get_ray(float s, float t);

	Vec lower_left;
	Vec x_comp;
	Vec y_comp;
	Vec origin;
	Vec u, v, w; // Unit vectors for horizontal, vertical, etc. directions.
	float lens_radius; // Defocus blur
};

#endif