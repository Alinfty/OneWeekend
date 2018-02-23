#pragma once

#ifndef __MULTI_

#define _USE_MATH_DEFINES //M_PI를 사용할 수 있게 해줌.
#include <cmath>
#include <algorithm>

class float4 {
public:
	float e[4];
	float &operator[](int n) {
		return e[n];
	}
	const float &operator[](int n) const {
		return e[n];
	}
	float4() {
		e[0] = e[1] = e[2] = e[3] = 0.f;
	}
	float4(float f) {
		e[0] = e[1] = e[2] = e[3] = f;
	}
	float4(float x, float y, float z, float w = 0) {
		e[0] = x;
		e[1] = y;
		e[2] = z;
		e[3] = w;
	}
	float x() { return e[0]; }
	float y() { return e[1]; }
	float z() { return e[2]; }
	float a() { return e[0]; }
	float b() { return e[1]; }
	float c() { return e[2]; }

	float4 operator-() { return float4(-e[0], -e[1], -e[2], -e[3]); }

	float4 &operator=(const float4 &b) { e[0] = b[0]; e[1] = b[1]; e[2] = b[2]; e[3] = b[3]; return *this; }
	float4 operator+ (const float4 &b) const { return float4(e[0] + b[0], e[1] + b[1], e[2] + b[2], e[3] + b[3]); }
	float4 operator- (const float4 &b) const { return float4(e[0] - b[0], e[1] - b[1], e[2] - b[2], e[3] - b[3]); }
	float4 operator* (const float4 &b) const { return float4(e[0] * b[0], e[1] * b[1], e[2] * b[2], e[3] * b[3]); }
	float4 operator* (float b) const { return float4(e[0] * b, e[1] * b, e[2] * b, e[3] * b); }
	float4 operator/ (float b) const { return float4(e[0] / b, e[1] / b, e[2] / b, e[3] / b); }

	float4 &operator+= (const float4 &b) { e[0] += b[0]; e[1] += b[1]; e[2] += b[2]; e[3] += b[3]; return *this; }
	float4 &operator-= (const float4 &b) { e[0] -= b[0]; e[1] -= b[1]; e[2] -= b[2]; e[3] -= b[3]; return *this; }
	float4 &operator*= (const float4 &b) { e[0] *= b[0]; e[1] *= b[1]; e[2] *= b[2]; e[3] *= b[3]; return *this; }
	float4 &operator*= (const float b) { e[0] *= b; e[1] *= b; e[2] *= b; e[3] *= b; return *this; }
	float4 &operator/= (const float b)  { e[0] /= b; e[1] /= b; e[2] /= b; e[3] /= b; return *this; }

	float4 &normalize() {
		float l = sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]);
		e[0] /= l; e[1] /= l; e[2] /= l; e[3] /= l;
		return *this;
	}
};

inline float4 operator* (const float b, const float4 &a) { return float4(b*a.e[0], b*a.e[1], b*a.e[2], b* a.e[3]); }

inline float dot(const float4 &a, const float4 &b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

inline float length2(const float4 &a) {
	return dot(a, a);
}

inline float length(const float4 &a) {
	return sqrt(dot(a, a));
}

inline float4 sqrt(const float4 &a) {
	return float4(sqrt(a[0]), sqrt(a[1]), sqrt(a[2]), sqrt(a[3]));
}

inline float4 min_element(const float4 &a, const float4 &b) {
	return float4(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]), std::min(a[3], b[3]));
}

inline float4 cross(const float4 &v1, const float4 &v2) {
	return float4(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1], v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2], v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline float4 unit_vector(const float4 &a){
	return a / length(a);
}

#else

#include <intrin.h>
#include <cmath>

class float4 {
public:
	__m128 f;
	float &operator[](int n) {
		return f.m128_f32[3 - n];
	}
	const float &operator[](int n) const {
		return f.m128_f32[3 - n];
	}
	float4() {
		f = _mm_setzero_ps();
	}
	float4(float x) {
		f = _mm_set1_ps(x);
	}
	float4(const __m128 &g) {
		f = g;
	}
	float4(float x, float y, float z, float w = 0.f) {
		f = _mm_set_ps(x, y, z, w);
	}

	operator __m128() const { return f; }
	float4 &operator=(const float4 &b) { f = b; return *this; }
	float4 operator+ (const float4 &b) const { return _mm_add_ps(f, b); }
	float4 operator- (const float4 &b) const { return _mm_sub_ps(f, b); }
	float4 operator* (const float4 &b) const { return _mm_mul_ps(f, b); }
	float4 operator* (const float b) const { return _mm_mul_ps(f, _mm_set1_ps(b)); }
	float4 operator/ (const float b) const { return _mm_div_ps(f, _mm_set1_ps(b)); }

	float4 &operator+= (const float4 &b) { f = _mm_add_ps(f, b); return *this; }
	float4 &operator-= (const float4 &b) { f = _mm_sub_ps(f, b); return *this; }
	float4 &operator*= (const float4 &b) { f = _mm_mul_ps(f, b); return *this; }
	float4 &operator*= (const float b) { f = _mm_mul_ps(f, _mm_set1_ps(b)); return *this; }
	float4 &operator/= (const float b) { f = _mm_div_ps(f, _mm_set1_ps(b)); return *this; }

	float4 &normalize() { f = _mm_div_ps(f, _mm_set_ps1(sqrt(_mm_dp_ps(f, f, 0xF1).m128_f32[0]))); return *this; }
};

inline float dot(const float4 &a, const float4 &b) {
	return _mm_dp_ps(a, b, 0xF1).m128_f32[0];
}

inline float length2(const float4 &a) {
	return _mm_dp_ps(a, a, 0xF1).m128_f32[0];
}

inline float length(const float4 &a) {
	return sqrt(_mm_dp_ps(a, a, 0xF1).m128_f32[0]);
}

inline float4 sqrt(const float4 &a) {
	return _mm_sqrt_ps(a);
}

inline float4 min_element(const float4 &a, const float4 &b) {
	return _mm_min_ps(a, b);
}

#endif


