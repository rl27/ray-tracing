#ifndef UTILITY_H
#define UTILITY_H

#include "Vec.h"

Vec unit_circle();
Vec unit_sphere();
Vec reflect(const Vec &v, const Vec &n);
bool refract(const Vec &v, const Vec &n, float ni_over_nt, Vec &refracted);
float schlick(float cos, float ref_idx);

#endif // !UTILITY_H
