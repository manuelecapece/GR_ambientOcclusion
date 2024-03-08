#pragma once
#include "color.h"
#include "common.h"
#include "texture.h"

class material {
public:
	color ca;
	color cd;
	color cs;
	float ka; //coefficiente di riflessione ambientale del materiale, ka appartiene a [0,1]
	float kd; //coefficiente di riflessione diffuso del materiale, kd appartiene a [0,1]

	material(color ambient, color diffuse, color specular, float cra, float crd) : ca(ambient), cd(diffuse), cs(specular), ka(cra) , kd(crd) {};

	material() {

		ca[0] = random_float();
		ca[1] = random_float();
		ca[2] = random_float();

		cd[0] = random_float();
		cd[1] = random_float();
		cd[2] = random_float();

		cs[0] = random_float();
		cs[1] = random_float();
		cs[2] = random_float();

		//ka = 20.0f + random_float() * 200.0f;
		//kd = 20.0f + random_float() * 200.0f;

		ka = 1.0f;
		kd = 0.0f;
	}

	texture* texture;
};




