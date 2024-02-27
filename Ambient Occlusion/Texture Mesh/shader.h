#pragma once
#include <iostream>
#include <time.h>
#include <algorithm>
#include "camera.h"
#include "light.h"
#include "material.h"
#include "object.h"
#include "color.h"
#include "vec3.h"
#include "math.h"
#include "hittable.h"
#include "ambient_occluder.h"
#include "spot_light.h"

inline vec3 reflect(const vec3& i, const vec3& n)
{
	//return i - 2.0f * dot(i, n) * n;
	return  2.0f * dot(i, n) * n - i; // i vettore uscente dalla normale
}

//Pong shading per point_light
color phong_shading(point_light& light, hit_record& hr, point3& camera_pos) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = hr.m->ca * light.ambient;

	vec3 L = unit_vector(light.position - hr.p);
	float LDotN = max(dot(L, hr.normal), 0.0f);

	if (LDotN > 0.0f) {
		//diffuse = hr.m->kd * light.diffuse * LDotN;
		diffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * light.diffuse * LDotN;

		vec3 R = reflect(L, hr.normal);

		vec3 V = unit_vector(camera_pos - hr.p);
		float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->ka);

		specular = hr.m->cs * light.specular * VDotR;

		return ambient + diffuse + specular;
	}
	else
		return ambient;
}

//Pong shading per spot_light
color phong_shading_spot(spot_light& light, hit_record& hr, point3& camera_pos) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = hr.m->ca * light.ambient;

	vec3 L = normalize(light.position - hr.p);
	vec3 Ls = normalize(light.direction);

	float LDotN = max(dot(L, hr.normal), 0.0f);
	float LsDotN = max(dot(Ls, L), 0.0f);


	if (LDotN > 0 && LsDotN > cos(light.apertureDegree)) {

		if (hr.m->texture != nullptr) {
			diffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * light.diffuse * LDotN * (pow(LsDotN, 70));
		}
		else {
			diffuse = hr.m->cd * light.diffuse * LDotN * (pow(LsDotN, 70));
		}

		vec3 R = reflect(L, hr.normal);

		vec3 V = unit_vector(camera_pos - hr.p);
		float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->ka);

		specular = hr.m->cs * light.specular * VDotR;

		return ambient + diffuse + specular;
	}
	else
		return ambient;
}



color ambient_shading(light &light, hit_record &hr) {
	color ambient(0.0f, 0.0f, 0.0f);
	
	ambient = hr.m->ca * light.ambient;

	return ambient;
}

color ambient_occlusion_shading(ambient_occluder& light, ray& r, hit_record& hr, hittable_list world) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);
	float ka = 0;
	float kd = 0;

	ambient  = hr.m->ca; //colore ambientale materiale
	diffuse  = hr.m->cd; //colore diffuso materiale
	specular = hr.m->cs; //colore speculare materiale
	ka       = hr.m->ka; //coefficiente di riflessione ambientle materiale
	kd       = hr.m->kd; //coefficiente di riflessione diffuso materiale

	vec3 wo = -r.d;
	color ambient_rho = ka * ambient;
	color L = ambient_rho * light.L(hr, world);

	return L;
}

//Phong shading per poit_light + ambient occlusion
color phong_shading(point_light& light, ambient_occluder& occluder, ray& r, hit_record& hr, hittable_list& world, point3& camera_pos) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = ambient_occlusion_shading(occluder, r, hr, world);

	vec3 L = unit_vector(light.position - hr.p);
	float LDotN = max(dot(L, hr.normal), 0.0f);

	if (LDotN > 0.0f) {
		//diffuse = hr.m->kd * light.diffuse * LDotN;
		diffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * light.diffuse * LDotN;

		vec3 R = reflect(L, hr.normal);

		vec3 V = unit_vector(camera_pos - hr.p);
		float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->ka);

		specular = hr.m->cs * light.specular * VDotR;

		return ambient + diffuse + specular;
	}
	else
		return ambient;
}

//Phong shading per spot_light + ambient occlusion
color phong_shading(spot_light& light, ambient_occluder& occluder, ray& r, hit_record& hr, hittable_list& world, point3& camera_pos) {
	color ambient(0.0f, 0.0f, 0.0f);
	color diffuse(0.0f, 0.0f, 0.0f);
	color specular(0.0f, 0.0f, 0.0f);

	ambient = ambient_occlusion_shading(occluder, r, hr, world);

	vec3 L = normalize(light.position - hr.p);
	vec3 Ls = normalize(light.direction);

	float LDotN = max(dot(L, hr.normal), 0.0f);
	float LsDotN = max(dot(Ls, L), 0.0f);


	if (LDotN > 0 && LsDotN > cos(light.apertureDegree)) {

		if (hr.m->texture != nullptr) {
			diffuse = hr.m->texture->value(hr.u, hr.v, hr.p) * light.diffuse * LDotN * (pow(LsDotN, 70));
		}
		else {
			diffuse = hr.m->cd * light.diffuse * LDotN * (pow(LsDotN, 70));
		}

		vec3 R = reflect(L, hr.normal);

		vec3 V = unit_vector(camera_pos - hr.p);
		float VDotR = std::pow(max(dot(V, R), 0.0f), hr.m->ka);

		specular = hr.m->cs * light.specular * VDotR;

		return ambient + diffuse + specular;
	}
	else
		return ambient;
}
