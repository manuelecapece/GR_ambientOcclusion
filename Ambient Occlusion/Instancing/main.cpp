#include <iostream>
#include <time.h>
#include <stdio.h>
#include "SDL.h" 
#include "SDL_image.h"
#include "float.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "geometry.h"
#include "cylinder.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "instance.h"
#include "mesh.h"
#include "raster.h"
#include "object.h"
#include "camera.h"
#include "color.h"
#include "texture.h"

const unsigned int MAX_RAY_DEPTH = 5;

using namespace std;

int init(int width, int height) {
	/* // Initialize SDL2. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	/* Create the window where we will draw. */
	window = SDL_CreateWindow("Phong Shading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	return 0;
}


void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	// World
	hittable_list world;

	//Definisco i colori
	color red        = color(1.0f, 0.0f, 0.0f);
	color green      = color(0.0f, 1.0f, 0.0f);
	color blu        = color(0.0f, 0.0f, 1.0f);
	color gray       = color(0.5f, 0.5f, 0.5f);
	color white      = color(1.0f, 1.0f, 1.0f);
	color black      = color(0.0f, 0.0f, 0.0f);
	color darkgray   = color(0.25f, 0.25f, 0.25f);
	color lightgray  = color(0.75f, 0.75f, 0.75f);
	color slategray  = color(0.4f, 0.5f, 0.56f);
	color magenta    = color(1.0f, 0.0f, 1.0f);
	color cyan       = color(0.0f, 1.0f, 1.0f);
	color brown      = color(0.60f, 0.40f, 0.12f);
	color yellow     = color(1.0f, 1.0f, 0.0f);
	color darkyellow = color(0.65f, 0.65f, 0.0f);
	
	//Sfera base
	sphere* sfera_base = new sphere(); //posizione (0,0,0) e raggio 1.0
	auto instace_ptr_base = make_shared<instance>(sfera_base, new material(gray, gray, gray, 1));
	instace_ptr_base->scale(1000.0f, 1000.0f, 1000.0f);
	instace_ptr_base->translate(0.0f, -1000.0f, 0.0f);
	instace_ptr_base->getMaterial()->reflective = 0.0f;
	world.add(instace_ptr_base);

	//Sfera principale
	sphere* sfera = new sphere(); //posizione (0,0,0) e raggio 1.0
	auto instance_ptr_sfera = make_shared<instance>(sfera, new material(darkyellow, darkyellow, white, 1));
	instance_ptr_sfera->translate(0.0f, 1.0f, 0.0f);
	instance_ptr_sfera->getMaterial()->reflective = 0.0f;
	world.add(instance_ptr_sfera);

	//Luce
	point3 light_position(0.0f, 6.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);

	//Camera
	camera cam;
	cam.lookfrom = point3(0, 6, 15);
	cam.lookat = point3(0, 0, 0);

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 1280;
	cam.samples_per_pixel = 4;
	cam.vfov = 20;

	cam.initialize();

	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n255\n";

	time_t start, end;
	time(&start);

	cam.parallel_render(world, *worldlight);
	//cam.render(world, *worldlight);

	time(&end);
	double dif = difftime(end, start);
	cout << "\n" << "Rendering time: " << dif << "\n";

	SDL_Event event;
	bool quit = false;

	/* Poll for events */
	while (SDL_PollEvent(&event) || (!quit)) {

		switch (event.type) {

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
				// cases for other keypresses

			case SDLK_s:
				saveScreenshotBMP("screenshot.bmp");
				cout << "Screenshot saved!" << endl;
				break;
			}
		}
	}

	close();
	return 0;
}
