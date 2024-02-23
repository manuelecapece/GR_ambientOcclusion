#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "interval.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include "common.h"

class sphere : public hittable {
public:
  sphere() {
    center = point3(0.0f, 0.0f, 0.0f);
    radius = 1.0f;
  }
  sphere(point3 _center, float _radius) : center(_center), radius(_radius) {}

  const double kEpsilon = 0.001;

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    //vec3 oc = r.origin() - center;
    //float a = r.direction().length_squared();
    //float half_b = dot(oc, r.direction());
    //float c = oc.length_squared() - radius * radius;

    //float discriminant = half_b * half_b - a * c;
    //if (discriminant < 0) return false;
    //float sqrtd = sqrt(discriminant);

    //// Find the nearest root that lies in the acceptable range.
    //float root = (-half_b - sqrtd) / a;
    //if (!ray_t.surrounds(root)) {
    //  root = (-half_b + sqrtd) / a;
    //  if (!ray_t.surrounds(root))
    //    return false;
    //}

    //rec.t = root;
    //rec.p = r.at(rec.t);
    //vec3 outward_normal = (rec.p - center) / radius;
    //rec.set_face_normal(r, outward_normal);

    //return true;


    double t;
    vec3 temp   = r.o - center;
    double a    = dot(r.d , r.d);
    double b    = 2.0 * dot(temp , r.d);
    double c    = dot(temp , temp) - radius * radius;
    double disc = b * b - 4.0 * a * c;

    if (disc < 0.0)
        return(false);
    else {
        double e = sqrt(disc);
        double denom = 2.0 * a;
        t = (-b - e) / denom;    // smaller root

        if (t > kEpsilon) {
            ray_t.min = t;
            rec.normal = (temp + t * r.d) / radius;
            rec.p = r.o + t * r.d;
            return (true);
        }

        t = (-b + e) / denom;    // larger root

        if (t > kEpsilon) {
            ray_t.min = t;
            rec.normal = (temp + t * r.d) / radius;
            rec.p = r.o + t * r.d;
            return (true);
        }
    }

    return (false);
  }

  bool hit_shadow(const ray& r, interval ray_t) const override {
    //vec3 oc = r.origin() - center;
    //float a = r.direction().length_squared();
    //float half_b = dot(oc, r.direction());
    //float c = oc.length_squared() - radius * radius;

    //float discriminant = half_b * half_b - a * c;
    //if (discriminant < 0) return false;
    //float sqrtd = sqrt(discriminant);

    //// Find the nearest root that lies in the acceptable range.
    //float root = (-half_b - sqrtd) / a;
    //if (!ray_t.surrounds(root)) {
    //  root = (-half_b + sqrtd) / a;
    //  if (!ray_t.surrounds(root))
    //    return false;
    //}

    //return true;

      double t;
      vec3 temp   = r.o - center;
      double a    = dot(r.d , r.d);
      double b    = 2.0 * dot(temp , r.d);
      double c    = dot(temp , temp) - radius * radius;
      double disc = b * b - 4.0 * a * c;

      if (disc < 0.0)
          return(false);
      else {
          double e = sqrt(disc);
          double denom = 2.0 * a;
          t = (-b - e) / denom;    // smaller root

          if (t > kEpsilon) {
              ray_t.min = t;
              return (true);
          }

          t = (-b + e) / denom;    // larger root

          if (t > kEpsilon) {
              ray_t.min = t;
              return (true);
          }
      }

      return (false);
  }

private:
  point3 center;
  float radius;
};

#endif