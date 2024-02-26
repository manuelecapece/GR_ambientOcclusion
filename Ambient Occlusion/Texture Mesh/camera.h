#ifndef CAMERA_H
#define CAMERA_H
#include <windows.h>
#include <ppl.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <vector>
#include "color.h"
#include "common.h"
#include "hittable_list.h"
#include "light.h"
#include "raster.h"
#include "shader.h"

using namespace std;
using namespace concurrency;

class camera {
public:
	float aspect_ratio;       // Ratio of image width over height
	int   image_width;        // Rendered image width in pixel count
	int   image_height;       // Rendered image height
	int   samples_per_pixel;  // Count of random samples for each pixel

	float  vfov = 90.0f;                             // Vertical view angle (field of view)
	point3 lookfrom = point3(0.0f, 0.0f, -1.0f);    // Point camera is looking from
	point3 lookat = point3(0.0f, 0.0f, 0.0f);       // Point camera is looking at
	vec3   vup = vec3(0.0f, 1.0f, 0.0f);            // Camera-relative "up" direction
	point3 camera_center;  // Camera center

	camera() {}

	camera(float _aspect_ratio, int _image_width) {
		aspect_ratio = _aspect_ratio;
		image_width = _image_width;

		// Calculate the image height, and ensure that it's at least 1.
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;
	}

	void initialize() {
		// Calculate the image height, and ensure that it's at least 1.
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		camera_center = lookfrom;

		// Determine viewport dimensions.
		float focal_length = (lookfrom - lookat).length();
		float theta = degrees_to_radians(vfov);
		float h = tan(theta / 2.0f);
		float viewport_height = 2.0f * h * focal_length;
		float viewport_width = viewport_height * (static_cast<float>(image_width) / image_height);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
		vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel.
		point3 viewport_upper_left = camera_center - (focal_length * w) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
	}


	void render(hittable_list& world, point_light& worldlight) {
		for (int j = 0; j < image_height; ++j) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; ++i) {
				color pixel_color(0.0f, 0.0f, 0.0f);

				for (int sample = 0; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, worldlight);
				}

				pixel_color /= samples_per_pixel;
				setColor(pixel_color[0], pixel_color[1], pixel_color[2]);
				setPixel(i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

	void parallel_render(hittable_list& world, point_light& worldlight) {
		vector<color> matrix(image_width * image_height);
		float pixel_i = 0;
		float tot_pixel = image_height * image_width;
		int percentuale_render = 0;

		parallel_for(int(0), image_height, [&](int j) {
			for (int i = 0; i < image_width; ++i) {
				color pixel_color(0.0f, 0.0f, 0.0f);

				for (int sample = 0; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, worldlight);
				}

				pixel_i = pixel_i + 1;
				int newPercentualeRender = static_cast<int> ((pixel_i / tot_pixel) * 100);
				if (percentuale_render != newPercentualeRender) {
					percentuale_render = newPercentualeRender;
					cout << "rendering: " << percentuale_render << "%" << endl;
				}

				pixel_color /= samples_per_pixel;
				matrix[j * image_width + i] = pixel_color;
			}
			});

		for (int j = 0; j < image_height; j++) {
			for (int i = 0; i < image_width; i++) {
				color pixel_color = matrix[j * image_width + i];
				setColor(pixel_color[0], pixel_color[1], pixel_color[2]);
				setPixel(i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

	//RENDER PER AMBIENT OCCLUSION + LUCI
	void parallel_render(hittable_list& world, point_light& worldlight, ambient_occluder& occluder) {
		vector<color> matrix(image_width * image_height);
		float pixel_i = 0;
		float tot_pixel = image_height * image_width;
		int percentuale_render = 0;

		parallel_for(int(0), image_height, [&](int j) {
			for (int i = 0; i < image_width; ++i) {
				color pixel_color(0.0f, 0.0f, 0.0f);

				for (int sample = 0; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, worldlight,occluder);
				}

				pixel_i = pixel_i + 1;
				int newPercentualeRender = static_cast<int> ((pixel_i / tot_pixel) * 100);
				if (percentuale_render != newPercentualeRender) {
					percentuale_render = newPercentualeRender;
					cout << "rendering: " << percentuale_render << "%" << endl;
				}

				pixel_color /= samples_per_pixel;
				matrix[j * image_width + i] = pixel_color;
			}
			});

		for (int j = 0; j < image_height; j++) {
			for (int i = 0; i < image_width; i++) {
				color pixel_color = matrix[j * image_width + i];
				setColor(pixel_color[0], pixel_color[1], pixel_color[2]);
				setPixel(i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

	//RENDER PER AMBIENT OCCLUSION
	void parallel_render_ambient_occlusion(hittable_list& world, ambient_occluder& worldlight) {
		vector<color> matrix(image_width * image_height);
		float pixel_i = 0;
		float tot_pixel = image_height * image_width;
		int percentuale_render = 0;

		parallel_for(int(0), image_height, [&](int j) {
			for (int i = 0; i < image_width; ++i) {
				color pixel_color(0.0f, 0.0f, 0.0f);

				for (int sample = 0; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color_ambient_occlusion(r, world, worldlight);
				}

				pixel_i = pixel_i + 1;
				int newPercentualeRender = static_cast<int> ((pixel_i / tot_pixel) * 100);
				if (percentuale_render != newPercentualeRender) {
					percentuale_render = newPercentualeRender;
					cout << "rendering: " << percentuale_render << "%" << endl;
				}

				pixel_color /= samples_per_pixel;
				matrix[j * image_width + i] = pixel_color;
			}
			});

		for (int j = 0; j < image_height; j++) {
			for (int i = 0; i < image_width; i++) {
				color pixel_color = matrix[j * image_width + i];
				setColor(pixel_color[0], pixel_color[1], pixel_color[2]);
				setPixel(i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

private:
	point3 pixel00_loc;    // Location of pixel 0, 0
	vec3   pixel_delta_u;  // Offset to pixel to the right
	vec3   pixel_delta_v;  // Offset to pixel below

	vec3   u, v, w;        // Camera frame basis vectors

	ray get_ray(int i, int j) const {
		// Get a randomly sampled camera ray for the pixel at location i,j.

		vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		vec3 pixel_sample = pixel_center + pixel_sample_square();

		point3 ray_origin = camera_center;
		vec3 ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	vec3 pixel_sample_square() const {
		// Returns a random point in the square surrounding a pixel at the origin.
		float px = -0.5 + random_float();
		float py = -0.5 + random_float();
		return (px * pixel_delta_u) + (py * pixel_delta_v);
	}

	color ray_color(const ray& r, hittable_list& world, point_light& worldlight) {
		hit_record rec;

		if (world.hit(r, interval(0, infinity), rec)) {
			ray shadow_ray(rec.p, unit_vector(worldlight.position - rec.p));
			float closest_light = (rec.p - worldlight.position).length();

			if (world.hit_shadow(shadow_ray, interval(0.01f, closest_light)))
				return ambient_shading(worldlight, rec);
			else
				return phong_shading(worldlight, rec, camera_center);
		}

		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		//return lerp(color(1.0f, 1.0f, 1.0f), color(0.5f, 0.7f, 1.0f), t);
		return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f);
	}

	//RAY COLOR PER AMBIENT OCCLUSION + LUCI
	color ray_color(ray& r, hittable_list& world, point_light& worldlight, ambient_occluder& occluder) {
		hit_record rec;

		if (world.hit(r, interval(0, infinity), rec)) {
			ray shadow_ray(rec.p, unit_vector(worldlight.position - rec.p));
			float closest_light = (rec.p - worldlight.position).length();

			if (world.hit_shadow(shadow_ray, interval(0.01f, closest_light)))
				return ambient_occlusion_shading(occluder, r, rec, world);
			else
				return phong_shading(worldlight, occluder, r, rec, world, camera_center);
		}

		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f);
	}

	//RAY COLOR PER AMBIENT OCCLUSION
	color ray_color_ambient_occlusion(ray& r, hittable_list& world, ambient_occluder& worldlight) {
		hit_record rec;

		if (world.hit(r, interval(0.0f, infinity), rec))
		{
			return ambient_occlusion_shading(worldlight, r, rec, world);

		}
		else {
			vec3 unit_direction = unit_vector(r.direction());
			float t = 0.5f * (unit_direction.y() + 1.0f);
			return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f);
		}

	}

};

#endif




