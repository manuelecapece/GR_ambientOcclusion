#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "vec3.h"
#include "light.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
  std::vector<shared_ptr<hittable>> objects;
  vector<point_light*> v_light;

  hittable_list() {}
  hittable_list(shared_ptr<hittable> object) { add(object); }

  void clear() { objects.clear(); }

  void add(shared_ptr<hittable> object) {
    objects.push_back(object);
  }

  void addLight(point_light* new_light) {
      v_light.push_back(new_light);
  }

  bool hit_shadow(const ray& r, interval ray_t) const override {
    hit_record temp_rec;

    for (const auto& object : objects) {
      if (object->hit_shadow(r, ray_t))
        return true;
    }
    return false;
  }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max;

    for (const auto& object : objects) {
      if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
      }
    }

    return hit_anything;
  }

};

#endif

