#ifndef RAYH
#define RAYH

#include "float4.h"

class ray {
public:
	ray() {}
	ray(const float4 &a, const float4 &b) :A(a), B(b) {}
	float4 origin() const { return A; }
	float4 direction() const { return B; }
	float4 point_at_parameter(float t) const { return A + B * t; }
	float4 A, B;
};

#endif