#pragma once
#include "vec3.h"
#include "color.h"
#include "light.h"
#include "common.h"

class spot_light : public light {
public:

	point3 position;
	vec3 direction;
	float apertureDegree;

	spot_light() {}
	spot_light(point3 p, vec3 v, float ad, color a, color d, color s) {
		position = p;
		direction = v;
		//apertureDegree = Deg2Rad * ad;
		apertureDegree = degrees_to_radians(ad);
		ambient = a;
		diffuse = d;
		specular = s;
	}


};
