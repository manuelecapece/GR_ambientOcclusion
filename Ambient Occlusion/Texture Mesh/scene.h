#pragma once
#include <windows.h>
#include <ppl.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <ppl.h>
#include "SDL.h"
#include "vector3D.h"
#include "point3D.h"
#include "raster.h"
#include "camera.h"
#include "light.h"
#include "color.h"
#include "shader.h"
#include "bvh.h"
#include "instance.h"

// Se si usano mesh triangolari non usare il BVH
#define use_bvh		// commenta per non usare bvh

using namespace std;
using namespace concurrency;

class scene {
public:
	int nx;
	int ny;
	int ns;

	vector<instance *> objs_vector;

	camera *cam;

	point_light *light;

	bvh_node *bvh_scene;

	scene() {}

	void addObject(instance *new_object) {
		objs_vector.push_back(new_object);
	}

	void addLight(point_light *new_light) {
		light = new_light;
	}

	void setCamera(point3D position, point3D lookat, vector3D up, float fov, int res_x, int res_y, int n_sampling) {
		nx = res_x;
		ny = res_y;
		ns = n_sampling;
		float ratio = float(res_x) / float(res_y);
		cam = new camera(position, lookat, up, fov, ratio);
	}

	void render();
	void parallel_render();

private:

	bool trace_ray(const ray& r, float t_min, float t_max, hit_record& rec) const;
	bool trace_shadow_ray(const ray& r, const float closest_light) const;
	color shot(ray &r);

};

color scene::shot(ray &r) {
	hit_record rec;

	if (trace_ray(r, 0.0f, FLT_MAX, rec))
	{
		ray shadow_ray(rec.p, normalize(light->position - rec.p));
		float closest_light = shadow_ray.t_at_point(light->position);

		if (trace_shadow_ray(shadow_ray, closest_light))
			return ambient_shading(*light, rec);
		else
			return phong_shading(*light, rec, *cam);
	}
	else {
		vector3D unit_direction = normalize(r.direction());
		float t = 0.5f * (unit_direction.y + 1.0f);
		return lerp(color(1.0f, 1.0f, 1.0f), color(0.5f, 0.7f, 1.0f), t);
	}
}



void scene::render() {
#ifdef use_bvh
	bvh_scene = new bvh_node(objs_vector, 0, objs_vector.size() - 1);
#endif	

	for (int j = 0; j < ny; j++) {
		for (int i = 0; i < nx; i++) {

			color col(0.0, 0.0, 0.0);

			for (int s = 0; s < ns; s++) {
				ray r = cam->get_ray(float((i + randZeroToOne()) / nx), float((j + randZeroToOne()) / ny));
				col += scene::shot(r);
			}

			col /= float(ns);
			col = color(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			setColor(col.r, col.g, col.b);
			setPixel(i, j);

		}
	}
	SDL_RenderPresent(renderer);
}


void scene::parallel_render(){
#ifdef use_bvh
	bvh_scene = new bvh_node(objs_vector, 0, objs_vector.size() - 1);
#endif	

	vector<color> matrix(nx*ny);
	parallel_for(int(0), ny, [&](int j) {
		for (int i = 0; i < nx; i++)
		{
			color col(0.0, 0.0, 0.0);

			for (int s = 0; s < ns; s++) {
				ray r = cam->get_ray(float((i + randZeroToOne()) / nx), float((j + randZeroToOne()) / ny));
				col += scene::shot(r);
			}

			col /= float(ns);
			col = color(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			matrix[j * nx + i] = col;
		}
	});

	for (int j = 0; j < ny; j++)
	{
		for (int i = 0; i < nx; i++)
		{
			color col = matrix[j * nx + i];
			setColor(col.r, col.g, col.b);
			setPixel(i, j);
		}
	}
	SDL_RenderPresent(renderer);
}

bool scene::trace_ray(const ray& r, float t_min, float t_max, hit_record& rec) const {
#ifndef use_bvh
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for (int i = 0; i < objs_vector.size(); i++) {
		if (objs_vector[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
#endif

#ifdef use_bvh
	return bvh_scene->hit(r, t_min, t_max, rec);
#endif
}

bool scene::trace_shadow_ray(const ray& r, const float closest_light) const {
#ifndef use_bvh
	for (int i = 0; i < objs_vector.size(); i++)
		if (objs_vector[i]->hit_shadow(r, 0.01f, closest_light))
			return true;
	
	return false;
#endif
	
#ifdef use_bvh
	hit_record temp_rec;

	if (bvh_scene->hit(r, 0.01f, closest_light, temp_rec))
	//if (bvh_scene->hit_shadow(r, t_min, closest_so_far))
		return true;

	return false;
#endif
}