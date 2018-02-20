#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include <iostream>
#include <random>
#include <functional>
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"

using namespace std;

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

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, float4 attenuation, ray &scattered) const = 0;
};

class lambertian :public material {
public:
	lambertian(const float4 &a) :albedo(a) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, float4 attenuation, ray &scattered) const {
		float4 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}
	float4 albedo;
};

float4 color(const ray& r, hitable *world) {
	hit_record rec;
	if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
		float4 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5f*color(ray(rec.p, target - rec.p), world);
	}
	else {
		float4 unit_direction = unit_vector(r.direction());
		float t = 0.5f*(unit_direction.y() + 1.f);
		return (1.0f - t)*float4(1.f, 1.f, 1.f) + t * float4(0.5f, 0.7f, 1.0f);
	}
}


int main() {

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	myrand = std::bind(distribution, generator);

	int nx = 200;
	int ny = 100;
	int ns = 100;
	char data[100][200][3] = { 0, };

	hitable *list[2];
	list[0] = new sphere(float4(0, 0, -1), 0.5);
	list[1] = new sphere(float4(0, -100.5, -1), 100);
	hitable *world = new hitable_list(list, 2);
	camera cam;
	for (int j = 0; j < ny; j++) {
		if(j%10==0) cout << j << endl;
		for (int i = 0; i < nx; i++) {
			float4 col(0);
			for (int s = 0; s < ns; s++) {
				float u = float(i+myrand()) / nx;
				float v = float(j+myrand()) / ny;
				ray r = cam.get_ray(u, v);
				float4 p = r.point_at_parameter(2.0);
				col += color(r, world);
			}
			col /= float(ns);
			col = sqrt(col);

			int ir = int(col[0]*255.99);
			int ig = int(col[1]*255.99);
			int ib = int(col[2]*255.99);
			data[ny - 1 - j][i][0] = ir;
			data[ny - 1 - j][i][1] = ig;
			data[ny - 1 - j][i][2] = ib;
		}
	}
	stbi_write_bmp("output.bmp", nx, ny, 3, data);
}

/*
#include <fstream>

int main() {

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	myrand = std::bind(distribution, generator);

	int nx = 200;
	int ny = 100;
	int ns = 100;
	char data[100][200][3] = { 0, };
	fstream fout("output2.ppm", fstream::out);
	fout << "P3\n" << nx << ' ' << ny << "\n255\n";

	hitable *list[2];
	list[0] = new sphere(float4(0, 0, -1), 0.5);
	list[1] = new sphere(float4(0, -100.5, -1), 100);
	hitable *world = new hitable_list(list, 2);
	camera cam;
	for (int j = ny-1; j >=0; j--) {
		if (j % 10 == 0) cout << j << endl;
		for (int i = 0; i < nx; i++) {
			float4 col(0);
			for (int s = 0; s < ns; s++) {
				float u = float(i + myrand()) / nx;
				float v = float(j + myrand()) / ny;
				ray r = cam.get_ray(u, v);
				float4 p = r.point_at_parameter(2.0);
				col += color(r, world);
			}
			col /= float(ns);

			int ir = int(col[0] * 255.99);
			int ig = int(col[1] * 255.99);
			int ib = int(col[2] * 255.99);
			fout << ir << ' ' << ig << ' ' << ib << endl;
		}
	}
}*/