#pragma once

#ifndef CAMERAH
#define CAMERAH

#include <random>
#include <functional>
#include "ray.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0, 1.0);
auto myrand = std::bind(distribution, generator);
const float MAXFLOAT = 1000000.f;

float4 random_in_unit_sphere() {
	float4 p;
	do {
		p = 2.f*float4(myrand(), myrand(), myrand()) - float4(1.f, 1.f, 1.f);
	} while (length2(p) >= 1.0);
	return p;
}

float4 random_in_unit_disk() {
	float4 p;
	do {
		p = 2.f*float4(myrand(), myrand(), 0) - float4(1.f, 1.f, 0.f);
	} while (length2(p) >= 1.0);
	return p;
}

class camera {
public:
	camera(float4 lookfrom, float4 lookat, float4 vup, float vfov, float aspect, float aperture, float focus_dist) {
		lens_radius = aperture / 2;
		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin - half_width * focus_dist* u - half_height * focus_dist*v - focus_dist * w;
		horizontal = 2 * half_width*focus_dist*u;
		vertical = 2 * half_height*focus_dist*v;

	}
	ray get_ray(float s, float t) {
		float4 rd = lens_radius * random_in_unit_disk();
		float4 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}
	float4 origin, lower_left_corner, horizontal, vertical, u, v, w;
	float lens_radius;
};

#endif