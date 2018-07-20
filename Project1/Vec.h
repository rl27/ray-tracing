#ifndef VEC_H
#define VEC_H

#include <cmath>

class Vec
{
public:
    Vec() { e[0] = 0; e[1] = 0; e[2] = 0; }
    Vec(float a, float b, float c) { e[0] = a; e[1] = b; e[2] = c; }

    // Below: arithmetic operations, can either modify or not modify the calling Vec. All functions return a new Vec.

	inline void operator=(const Vec &v) { e[0] = v.e[0]; e[1] = v.e[1]; e[2] = v.e[2]; }

	// These functions do not change the calling vector
    inline Vec operator+(const Vec &v) const { return Vec(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]); }
    inline Vec operator-(const Vec &v) const { return Vec(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]); }
    inline Vec operator*(const Vec &v) const { return Vec(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]); }
    inline Vec operator/(const Vec &v) const { return Vec(e[0] / v.e[0], e[1] / v.e[1], e[2] / v.e[2]); }
    inline Vec operator*(const float f) const { return Vec(e[0]*f, e[1]*f, e[2]*f); }
    inline Vec operator/(const float f) const { return Vec(e[0]/f, e[1]/f, e[2]/f); }

	// These functions directly change the calling vector
    inline Vec& operator+=(const Vec &v) { *this = *this + v; return *this; }
    inline Vec& operator-=(const Vec &v) { *this = *this - v; return *this; }
    inline Vec& operator*=(const Vec &v) { *this = *this * v; return *this; }
    inline Vec& operator/=(const Vec &v) { *this = *this / v; return *this; }
    inline Vec& operator*=(const float f) { *this = *this * f; return *this; }
    inline Vec& operator/=(const float f) { *this = *this / f; return *this; }

    // Various functions
    float length() const { return sqrt(pow(e[0], 2) + pow(e[1], 2) + pow(e[2], 2)); }
	float squared_length() const { return pow(e[0], 2) + pow(e[1], 2) + pow(e[2], 2); }
    inline float dot(const Vec &v) const { return e[0]*v.e[0] + e[1]*v.e[1] + e[2]*v.e[2]; }
    inline Vec cross(const Vec &v) const { return Vec( (e[1]*v.e[2] - e[2]*v.e[1]), (e[2]*v.e[0] - e[0]*v.e[2]), (e[0]*v.e[1] - e[1]*v.e[0])); }
    inline Vec unit_vec() const { return *this / (this->length()); }

    float e[3];
};

#endif
