#pragma once
#include "vec3.h"
#include "color.h"
#include "math.h"
#include "common.h"

class light {
public:
	color ambient;
	color diffuse;
	color specular;

	float att_a;
	float att_b;
	float att_c;
};

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

	virtual void f();
};

class spot_light : public point_light {
public:

	vec3 direction;
	float apertureDegree;

	spot_light() {}
	spot_light(point3 p, vec3 v, float ad, color a, color d, color s) {
		position = p;
		direction = v;
		apertureDegree = Deg2Rad * ad;
		ambient = a;
		diffuse = d;
		specular = s;
	}


};

void point_light::f()
{
	return;
}
