#pragma once

#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

class camera {
public:
	camera() {
		lower_left_corner = float4(-2.0, -1.0, -1.0);
		horizontal = float4(4.0, 0.0, 0.0);
		vertical = float4(0.0, 2.0, 0.0);
		origin = float4(0.0, 0.0, 0.0);
	}
	ray get_ray(float u, float v) {
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}
	float4 origin, lower_left_corner, horizontal, vertical;
};

#endif