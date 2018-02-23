#define _USE_MATH_DEFINES //M_PI를 사용할 수 있게 해줌.

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include <iostream>
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"

using namespace std;



class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, float4 &attenuation, ray &scattered) const = 0;
};

class lambertian :public material {
public:
	lambertian(const float4 &a) :albedo(a) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, float4 &attenuation, ray &scattered) const {
		float4 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	float4 albedo;
};

float4 reflect(const float4 &v, const float4 &n) {
	return v - 2 * dot(v, n)*n;
}

bool refract(const float4 &v, const float4 &n, float ni_over_nt, float4& reflected) {
	float4 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.f - ni_over_nt * (1 - dt * dt);
	if (discriminant > 0) {
		reflected = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}

float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	//return r0 + (1 - r0)*pow((1 - cosine), 5);
	//pow는 느려서 Fresnel reflectance를 안 쓴 보람이 없음.
	return r0 + (1 - r0)* (1 - cosine)*(1 - cosine)*(1 - cosine)*(1 - cosine)*(1 - cosine);
}

class metal : public material {
public:
	metal(const float4 &a):albedo(a){}
	metal(const float4 &a, float f) :albedo(a), fuzz(f) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, float4 &attenuation, ray &scattered) const {
		float4 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	float4 albedo;
	float fuzz;
};

class dielectric : public material {
public:
	dielectric(float ri) :ref_idx(ri) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, float4 &attenuation, ray &scattered) const {
		float4 outward_normal;
		float4 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = float4(1.f, 1.f, 1.f);
		float4 refracted;
		float reflect_probe;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = float4(0) - rec.normal;
			ni_over_nt = ref_idx;
			//cosine =  ref_idx * dot(r_in.direction(), rec.normal) / length(r_in.direction());
			//Fresnel reflectance. schlick의 근사는 두 각 중 큰 각을 집어넣어야 한다. 그렇지 않다면 굴절율이 서로 바뀌어도 같은 결과가 나올것.
			//따라서 안에서 밖으로 나갈 때는 cos값을 바꾸어 주어야 한다.
			//자세한 계산은 Reflections and Refractions in Ray Tracing by Bram de Greve 참고
			//cosine 계산한 코드에 유리구에 원형으로 선이 생기는 문제가 있음. 그게 정상일지도?
			cosine= dot(r_in.direction(), rec.normal) / length(r_in.direction());
			cosine = sqrt(1 - ref_idx * ref_idx*(1 - cosine * cosine));
			//cosine = ref_idx*dot(r_in.direction(), rec.normal) / length(r_in.direction());
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1 / ref_idx;
			cosine = - dot(r_in.direction(), rec.normal) / length(r_in.direction());
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_probe = schlick(cosine, ref_idx);
		}
		else {
			reflect_probe = 1.f;
		}
		if (myrand() < reflect_probe) {
			scattered = ray(rec.p, reflected);
		}
		else {
			scattered = ray(rec.p, refracted);
		}
		return true;
	}
	float ref_idx;
};

float4 color(const ray& r, hitable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
		ray scattered;
		float4 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else return float4(0);
	}
	else {
		float4 unit_direction = unit_vector(r.direction());
		float t = 0.5f*(unit_direction.y() + 1.f);
		return (1.0f - t)*float4(1.f, 1.f, 1.f) + t * float4(0.5f, 0.7f, 1.0f);
	}
}

hitable *random_scene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(float4(0, -1000, 0), 1000, new lambertian(float4(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = myrand();
			float4 center(a + 0.9*myrand(), 0.2, b + 0.9*myrand());
			if (length(center - float4(4, 0.2f, 0)) > 0.9) {
				if (choose_mat < 0.8) {
					list[i++] = new sphere(center, 0.2, new lambertian(float4(myrand()*myrand(), myrand()*myrand(), myrand()*myrand())));
				}
				else if (choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2, new metal(float4(0.5*(1 + myrand()), 0.5*(1 + myrand()), 0.5*(1 + myrand())), 0.5*myrand()));
				}
				else {
					list[i++] = new sphere(center, 0.2, new dielectric(1.5f));
				}
			}
		}
	}
	list[i++] = new sphere(float4(0, 1, 0), 1.f, new dielectric(1.5));
	list[i++] = new sphere(float4(-4, 1, 0), 1.f, new lambertian(float4(0.4f, 0.2f, 0.1f)));
	list[i++] = new sphere(float4(4, 1, 0), 1.f, new metal(float4(0.7f, 0.6f, 0.5f), 0.f));

	return new hitable_list(list, i);
}


int main() {

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	myrand = std::bind(distribution, generator);

	int nx = 400;
	int ny = 200;
	int ns = 100;
	char data[200][400][3] = { 0, };

	float4 lookfrom(12.f, 2.f, 3.f);
	float4 lookat(0, 0, -1);
	float dist_to_fucus = length(lookfrom - lookat);
	float aperture = 0.0f;

	//hitable *list[5];
	//list[0] = new sphere(float4(0, 0, -1), 0.5, new lambertian(float4(0.1f, 0.2f, 0.5f)));
	//list[1] = new sphere(float4(0, -100.5, -1), 100, new lambertian(float4(0.8f, 0.8f, 0.f)));
	//list[2] = new sphere(float4(1, 0, -1), 0.5f, new metal(float4(0.8f, 0.6f, 0.2f), 0.f));
	//list[3] = new sphere(float4(-1, 0, -1), 0.5f, new dielectric(1.5f));
	//list[4] = new sphere(float4(-1, 0, -1), -0.45f, new dielectric(1.5f));
	//hitable *world = new hitable_list(list, 4);
	hitable *world = random_scene();
	camera cam(lookfrom, lookat, float4(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_fucus);
	for (int j = 0; j < ny; j++) {
		if(j%10==0) cout << j << endl;
		for (int i = 0; i < nx; i++) {
			float4 col(0);
			for (int s = 0; s < ns; s++) {
				float u = float(i+myrand()) / nx;
				float v = float(j+myrand()) / ny;
				ray r = cam.get_ray(u, v);
				float4 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
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
	stbi_write_bmp("oneweekend1.bmp", nx, ny, 3, data);
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