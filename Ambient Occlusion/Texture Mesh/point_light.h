#pragma once
#include "vec3.h"
#include "color.h"
#include "light.h"


class point_light : public light {
public:
	point3 position;

	point_light() {}
	point_light(point3 p, color a, color d, color s) {
		position = p;
		ambient = a;
		diffuse = d;
		specular = s;
	}
};