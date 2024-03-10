#pragma once

#include "vec3.h"
#include "color.h"
#include "sampler.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "color.h"
#include "light.h"

class ambient_occluder : public light {
public:
    vec3 u, v, w;
    sampler* sampler_ptr;
    color min_amount;

    ambient_occluder() {}

    ambient_occluder(color cla, color cld, color cls, float ls) {
        ambient = cla;
        diffuse = cld;
        specular = cls;
        att_a = ls;
    }

    vec3 get_direction(hit_record& sr);
    bool in_shadow(ray& ray, hit_record& sr, hittable_list& world);
    color L(hit_record& sr, hittable_list& world);
    void set_sampler(sampler* s_ptr);
    void set_min_amount(float a);

};

vec3 ambient_occluder::get_direction(hit_record& sr) {
    point3 sp = sampler_ptr->sample_hemisphere();
    return (sp.e[0] * u + sp.e[1] * v + sp.e[2] * w);
}

bool ambient_occluder::in_shadow(ray& ray, hit_record& sr, hittable_list& world) {

    interval t = interval(0.01f, infinity);
    float closest_so_far = t.max;
    int num_objects = world.objects.size();
    for (int j = 0; j < num_objects; j++) {
        if (world.objects[j]->hit_shadow(ray, t)) {
            return true;
        }
    }
    return false;

}

color ambient_occluder::L(hit_record& sr, hittable_list& world) {
    w = sr.normal;
    v = cross(w, vec3(0.00072, 1.0, 0.0034));
    v = normalize(v);
    u = cross(v, w);

    ray shadow_ray;
    shadow_ray.o = sr.p;
    shadow_ray.d = get_direction(sr);

    if (in_shadow(shadow_ray, sr, world)) {
        return (min_amount * att_a * ambient);
    }
    else {
        return (att_a * ambient);
    }
}

void ambient_occluder::set_sampler(sampler* s_ptr) {
    if (sampler_ptr) {
        delete sampler_ptr;
        sampler_ptr = NULL;
    }
    sampler_ptr = s_ptr;
    sampler_ptr->map_samples_to_hemisphere(1);
}

void ambient_occluder::set_min_amount(float a) {
    min_amount = color(a, a, a);
}