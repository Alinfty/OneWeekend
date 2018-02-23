#pragma once
#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class material;

class sphere : public hitable {
public:
	sphere() {}
	sphere(float4 cen, float r) :center(cen), radius(r) {}
	sphere(float4 cen, float r, material *mat) :center(cen), radius(r), mat_ptr(mat) {}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	float4 center;
	float radius;
	material *mat_ptr;
};

bool sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
	float4 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (tmin <= temp && temp <= tmax) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (tmin <= temp && temp <= tmax) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

#endif