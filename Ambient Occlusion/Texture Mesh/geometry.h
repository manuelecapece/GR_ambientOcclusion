#pragma once
#include "object.h"
#include "math.h"
#include "vec3.h"
#include "material.h"
#include "common.h"

class plane : public hittable {
public:
	plane() {}
	
	plane(const point3 p, const vec3 &n) {
		point = p;
		normal = n;
}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		float t = dot((point - r.o), normal) / dot(r.d, normal);

		if (t > kEpsilon) {
			ray_t.min = t;
			rec.normal = normal;
			rec.p = r.o + t * r.d;

			return (true);
		}

		return(false);
	}

	bool hit_shadow(const ray& r, interval ray_t) const override {
		float t = dot((point - r.o), normal) / dot(r.d, normal);
		if (t > kEpsilon) {
			ray_t.min = t;
			return true;
		}
		else return false;
	}

private:
	point3 point;
	vec3 normal;
};


