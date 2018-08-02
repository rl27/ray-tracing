#ifndef UTILITY_H
#define UTILITY_H

#include "Vec.h"

Vec random_on_sphere(const Vec &c, const float &r);
Vec unit_circle();
Vec unit_sphere();
Vec hemisphere(Vec n);
Vec reflect(const Vec &v, const Vec &n);
bool refract(const Vec &v, const Vec &n, float ni_over_nt, Vec &refracted);
float schlick(float cos, float ref_idx);
float clamp(float x);

#endif // !UTILITY_H
