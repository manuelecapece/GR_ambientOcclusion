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
#include "sampler.h"
#include "regular.h"
#include "multiJittered.h"
#include "ambient_occluder.h"

using namespace std;

int init(int width, int height) {
	/* // Initialize SDL2. */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	/* Create the window where we will draw. */
	window = SDL_CreateWindow("Texture Mesh", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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

color getColor(string colore) {

	if (colore == "red") {
		return color(1.00f, 0.00f, 0.00f);
	}
	if (colore == "green") {
		return color(0.00f, 1.00f, 0.00f);
	}
	if (colore == "blue") {
		return color(0.00f, 0.00f, 1.00f);
	}
	if (colore == "gray") {
		return color(0.50f, 0.50f, 0.50f);
	}
	if (colore == "white") {
		return color(1.00f, 1.00f, 1.00f);
	}
	if (colore == "black") {
		return color(0.00f, 0.00f, 0.00f);
	}
	if (colore == "darkgray") {
		return color(0.25f, 0.25f, 0.25f);
	}
	if (colore == "lightgray") {
		return color(0.75f, 0.75f, 0.75f);
	}
	if (colore == "slategray") {
		return color(0.40f, 0.50f, 0.56f);
	}
	if (colore == "magenta") {
		return color(1.00f, 0.00f, 1.00f);
	}
	if (colore == "cyan") {
		return color(0.00f, 1.00f, 1.00f);
	}
	if (colore == "brown") {
		return color(0.60f, 0.40f, 0.12f);
	}
	if (colore == "yellow") {
		return color(0.65f, 0.65f, 0.00f);
	}

}

void sphereMultiJittered(camera cam, hittable_list world) {
	
	//Luce ambient occluder
	ambient_occluder* occluder_ptr = new ambient_occluder(getColor("white"),getColor("white"),getColor("white"),1.0);
	occluder_ptr->set_min_amount(0.0);

	//Cube base
	mesh* cube = new mesh("../models/cube.obj", "../models/");
	texture* colore_cubo = new image_texture("../models/grigio.jpg");
	material* m_cube = new material(getColor("white"), getColor("white"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
	m_cube->texture = colore_cubo;
	world.add(instance_ptr_cubo);

	//Sfera principale obj
	mesh* sfera_obj = new mesh("../models/sfera.obj", "../models/");
	texture* colore_sfera_obj = new image_texture("../models/giallo.jpg");
	material* m_sfera_obj = new material(getColor("yellow"), getColor("yellow"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_sfera_obj = make_shared<instance>(sfera_obj, m_sfera_obj);
	instance_ptr_sfera_obj->scale(0.3, 0.3, 0.3);
	m_sfera_obj->texture = colore_sfera_obj;
	world.add(instance_ptr_sfera_obj);

	//Camera
	cam.lookfrom = point3(0, 5, 10);
	cam.lookat = point3(0, 0.5, 0);

	int num_samples = 0;

	std::vector<int> vec_samples = { 1, 16, 64, 256 };

	for (int i = 0; i < vec_samples.size(); i++) {
		//Sampler setting
		num_samples = vec_samples[i];
		cam.samples_per_pixel = num_samples;
		multiJittered* sampler_ptr = new multiJittered(num_samples);
		occluder_ptr->set_sampler(sampler_ptr);

		cam.initialize();
		cam.parallel_render_ambient_occlusion(world, *occluder_ptr);
		SDL_RenderPresent(renderer);

		string path = "../../screenshot/sphereMultiJittered/num_samples-" + to_string(num_samples) + ".bmp";
		saveScreenshotBMP(path);
	}
}

void sphereRegular(camera cam, hittable_list world) {

	//Luce ambient occluder
	ambient_occluder* occluder_ptr = new ambient_occluder(getColor("white"), getColor("white"), getColor("white"), 1.0);
	occluder_ptr->set_min_amount(0.0);

	//Cube base
	mesh* cube = new mesh("../models/cube.obj", "../models/");
	texture* colore_cubo = new image_texture("../models/grigio.jpg");
	material* m_cube = new material(getColor("white"), getColor("white"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
	m_cube->texture = colore_cubo;
	world.add(instance_ptr_cubo);

	//Sfera principale obj
	mesh* sfera_obj = new mesh("../models/sfera.obj", "../models/");
	texture* colore_sfera_obj = new image_texture("../models/giallo.jpg");
	material* m_sfera_obj = new material(getColor("yellow"), getColor("yellow"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_sfera_obj = make_shared<instance>(sfera_obj, m_sfera_obj);
	instance_ptr_sfera_obj->scale(0.3, 0.3, 0.3);
	m_sfera_obj->texture = colore_sfera_obj;
	world.add(instance_ptr_sfera_obj);

	//Camera
	cam.lookfrom = point3(0, 5, 10);
	cam.lookat = point3(0, 0.5, 0);

	int num_samples = 0;

	std::vector<int> vec_samples = { 1, 16, 64, 256 };

	for (int i = 0; i < vec_samples.size(); i++) {
		//Sampler setting
		num_samples = vec_samples[i];
		cam.samples_per_pixel = num_samples;
		regular* sampler_ptr = new regular(num_samples);
		occluder_ptr->set_sampler(sampler_ptr);

		cam.initialize();
		cam.parallel_render_ambient_occlusion(world, *occluder_ptr);
		SDL_RenderPresent(renderer);

		string path = "../../screenshot/sphereRegular/num_samples-" + to_string(num_samples) + ".bmp";
		saveScreenshotBMP(path);
	}
}

void multipleObjects(camera cam, hittable_list world) {

	//Colori texture
	texture* verde = new image_texture("../models/verde.jpg");
	texture* giallo = new image_texture("../models/giallo.jpg");
	texture* rosso = new image_texture("../models/rosso.jpg");
	texture* blu = new image_texture("../models/blu.jpg");
	texture* grigio = new image_texture("../models/grigio.jpg");
	texture* bianco = new image_texture("../models/grigio.jpg");

	//Sampler
	int num_samples = 64;
	float min_amount = 0.45f;
	multiJittered* sampler_ptr = new multiJittered(num_samples);

	//Luce ambient occluder
	ambient_occluder* occluder_ptr = new ambient_occluder(getColor("white"), getColor("white"), getColor("white"), 1.0);
	occluder_ptr->set_min_amount(min_amount);
	occluder_ptr->set_sampler(sampler_ptr);

	//Cube base
	mesh* cube = new mesh("../models/cube.obj", "../models/");
	material* m_cube = new material(getColor("lightgray"), getColor("lightgray"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
	m_cube->texture = grigio;
	world.add(instance_ptr_cubo);

	//Cube muro1
	material* m_cube2 = new material(getColor("green"), getColor("green"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo2 = make_shared<instance>(cube, m_cube2);
	instance_ptr_cubo2->scale(0.1, 3.0, 5.0);
	instance_ptr_cubo2->translate(-2.3f, 1.5f, -0.5f);
	m_cube2->texture = verde;
	world.add(instance_ptr_cubo2);

	//Cube muro3
	material* m_cube3 = new material(getColor("yellow"), getColor("yellow"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo3 = make_shared<instance>(cube, m_cube3);
	instance_ptr_cubo3->scale(0.1, 3.0, 4.6);
	instance_ptr_cubo3->translate(3.0f, 1.5f, 0.0f);
	instance_ptr_cubo3->rotate_y(90.0f);
	m_cube3->texture = giallo;
	world.add(instance_ptr_cubo3);

	//Coniglio 
	mesh* bunny = new mesh("../models/bunny.obj", "../models/");
	texture* colore_bunny = new image_texture("../models/bianco.jpg");
	material* m_bunny = new material(getColor("blue"), getColor("blue"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_bunny = make_shared<instance>(bunny, m_bunny);
	instance_ptr_bunny->rotate_y(45.0f);
	instance_ptr_bunny->translate(-1.0f, 0.0f, -2.0f);
	m_bunny->texture = colore_bunny;
	world.add(instance_ptr_bunny);

	//Luce point_light
	point3 light_position(0.0f, 10.0f, 0.0f);
	point_light* worldlight = new point_light(light_position, getColor("lightgray"), getColor("lightgray"), getColor("lightgray"));
	world.add_lights(worldlight);

	cam.lookfrom = point3(1, 2, 3.5);
	cam.lookat = point3(-0.5, 1.2, 0);

	cam.samples_per_pixel = 1;
	cam.initialize();

	cam.parallel_render(world, *worldlight);
	//cam.parallel_render_ambient_occlusion(world, *occluder_ptr);
	SDL_RenderPresent(renderer);
	string path = "../../screenshot/multipleObjects/num_samples-" + to_string(num_samples) +"-min_amount-"+ to_string(min_amount) + ".bmp";
	saveScreenshotBMP(path);
}

void city(camera cam, hittable_list world) {

	//Colori texture
	texture* verde = new image_texture("../models/verde.jpg");
	texture* giallo = new image_texture("../models/giallo.jpg");
	texture* rosso = new image_texture("../models/rosso.jpg");
	texture* blu = new image_texture("../models/blu.jpg");
	texture* grigio = new image_texture("../models/grigio.jpg");
	texture* bianco = new image_texture("../models/grigio.jpg");

	//Sampler
	int num_samples = 1;
	float min_amount = 0.25f;
	multiJittered* sampler_ptr = new multiJittered(num_samples);

	//Luce ambient occluder
	ambient_occluder* occluder_ptr = new ambient_occluder(getColor("darkgray"), getColor("lightgray"), getColor("black"), 1.0);
	occluder_ptr->set_min_amount(min_amount);
	occluder_ptr->set_sampler(sampler_ptr);

	//Luce point_light
	point3 light_position(-1.0f, 4, 2.0f);
	point_light* worldlight = new point_light(light_position, getColor("darkgray"), getColor("lightgray"), getColor("black"));
	world.add_lights(worldlight);

	//Cube base
	mesh* cube = new mesh("../models/cube.obj", "../models/");
	material* m_cube = new material(getColor("white"), getColor("white"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
	m_cube->texture = bianco;
	world.add(instance_ptr_cubo);

	srand(10);

	auto instance_ptr = make_shared<instance>(cube, new material());

	for (int a = -11; a < 5; a++) {
		for (int b = -11; b < 1; b++) {
			point3 center(a + 0.9f * random_float(), 0.1f, b + 0.9f * random_float());
			material* m_randomCube = new material();
			instance_ptr = make_shared<instance>(cube, m_randomCube);
			instance_ptr->scale(0.2f, 0.2f, 0.2f);
			instance_ptr->translate(center[0], center[1], center[2]);
			m_randomCube->texture = blu;
			world.add(instance_ptr);
		}
	}

	for (int a = -11; a < 5; a++) {
		for (int b = -11; b < 1; b++) {
			point3 center(a + 0.9f * random_float(), 0.5f, b + 0.9f * random_float());
			material* m_randomCube = new material();
			instance_ptr = make_shared<instance>(cube, m_randomCube);
			instance_ptr->scale(0.4f, 1.0f, 0.2f);
			instance_ptr->translate(center[0], center[1], center[2]);
			m_randomCube->texture = blu;
			world.add(instance_ptr);
		}
	}

	for (int a = -11; a < 5; a++) {
		for (int b = -11; b < -5; b++) {
			point3 center(a + 0.9f * random_float(), 1.0f, b + 0.9f * random_float());
			material* m_randomCube = new material();
			instance_ptr = make_shared<instance>(cube, m_randomCube);
			instance_ptr->scale(0.5f, 2.0f, 0.5f);
			instance_ptr->translate(center[0], center[1], center[2]);
			m_randomCube->texture = blu;
			world.add(instance_ptr);
		}
	}

	cam.lookfrom = point3(-3, 15, 15.0f);
	cam.lookat = point3(-3, 0, -5);

	cam.samples_per_pixel = num_samples;
	cam.initialize();

	//cam.parallel_render(world, *worldlight);
	cam.parallel_render_ambient_occlusion(world, *occluder_ptr);
	SDL_RenderPresent(renderer);
	string path = "../../screenshot/city/num_samples-" + to_string(num_samples) + "-min_amount-" + to_string(min_amount) + ".bmp";
	saveScreenshotBMP(path);

}

void multipleLights(camera cam, hittable_list world) {

	//Colori texture
	texture* verde  = new image_texture("../models/verde.jpg");
	texture* giallo = new image_texture("../models/giallo.jpg");
	texture* rosso  = new image_texture("../models/rosso.jpg");
	texture* blu    = new image_texture("../models/blu.jpg");
	texture* grigio = new image_texture("../models/grigio.jpg");
	texture* bianco = new image_texture("../models/grigio.jpg");

	//Sampler
	int num_samples = 1;
	float min_amount = 0.25f;
	multiJittered* sampler_ptr = new multiJittered(num_samples);
	//regular* sampler_ptr = new regular(num_samples);

	//Luce ambient occluder
	ambient_occluder* occluder_ptr = new ambient_occluder(getColor("darkgray"), getColor("lightgray"), getColor("black"), 1.0);
	occluder_ptr->set_min_amount(min_amount);
	occluder_ptr->set_sampler(sampler_ptr);

	//Luce point_light
	point3 light_position(-1.0f, 4, 2.0f);
	point_light* worldlight = new point_light(light_position, getColor("darkgray"), getColor("lightgray"), getColor("black"));
	world.add_lights(worldlight);

	//Cube base
	mesh* cube = new mesh("../models/cube.obj", "../models/");
	material* m_cube = new material(getColor("white"), getColor("white"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
	m_cube->texture = bianco;
	world.add(instance_ptr_cubo);

	//Cube sopra
	material* m_cube2 = new material(getColor("blue"), getColor("blue"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo2 = make_shared<instance>(cube, m_cube2);
	instance_ptr_cubo2->scale(1.4, 2.4, 1.4);
	instance_ptr_cubo2->translate(1.2f, 1.0f, -1.0f);
	m_cube2->texture = blu;
	world.add(instance_ptr_cubo2);

	//Cube muro1
	material* m_cube3 = new material(getColor("red"), getColor("red"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo3 = make_shared<instance>(cube, m_cube3);
	instance_ptr_cubo3->scale(0.1, 3.0, 5.0);
	instance_ptr_cubo3->translate(-2.3f, 1.5f, -0.5f);
	m_cube3->texture = rosso;
	world.add(instance_ptr_cubo3);

	//Cube muro2
	material* m_cube4 = new material(getColor("green"), getColor("green"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo4 = make_shared<instance>(cube, m_cube4);
	instance_ptr_cubo4->scale(0.1, 3.0, 5.0);
	instance_ptr_cubo4->translate(2.3f, 1.5f, -0.5f);
	m_cube4->texture = verde;
	world.add(instance_ptr_cubo4);

	//Cube muro3
	material* m_cube5 = new material(getColor("white"), getColor("white"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_cubo5 = make_shared<instance>(cube, m_cube5);
	instance_ptr_cubo5->scale(0.1, 3.0, 4.6);
	instance_ptr_cubo5->translate(3.0f, 1.5f, 0.0f);
	instance_ptr_cubo5->rotate_y(90.0f);
	m_cube5->texture = bianco;
	world.add(instance_ptr_cubo5);

	//Palla obj
	mesh* sfera_obj = new mesh("../models/futbol.obj", "../models/");
	texture* balltex = new image_texture("../models/BallYellow.jpg");
	material* m_sfera_obj = new material(getColor("yellow"), getColor("yellow"), getColor("white"), 0.8f, 0.0f);
	auto instance_ptr_sfera_obj = make_shared<instance>(sfera_obj, m_sfera_obj);
	instance_ptr_sfera_obj->scale(0.06, 0.06, 0.06);
	instance_ptr_sfera_obj->translate(-1.0f, 0.6f, 0.0f);
	m_sfera_obj->texture = balltex;
	world.add(instance_ptr_sfera_obj);

	cam.lookfrom = point3(0, 2, 9.0f);
	cam.lookat = point3(0, 1.1, 0);

	cam.samples_per_pixel = num_samples;
	cam.initialize();

	//cam.parallel_render(world, *worldlight);
	cam.parallel_render(world, *worldlight, *occluder_ptr);
	SDL_RenderPresent(renderer);
	string path = "../../screenshot/multipleLights/num_samples-" + to_string(num_samples) + ".bmp";
	saveScreenshotBMP(path);
}

int main(int argc, char* argv[])
{
	hittable_list world;
	camera cam;

	cam.aspect_ratio = 16.0f / 9.0f;
	cam.image_width = 960;
	cam.samples_per_pixel = 4;
	cam.vfov = 20;

	cout << "--- Menu" << endl;
	cout << "* Press a for random sampling sphere-plane" << endl;
	cout << "* Press b for regular sampling sphere-plane" << endl;
	cout << "* Press c for multiple objects scene" << endl;
	cout << "* Press d for multiple lights scene" << endl;
	cout << "* Press e for ambient occlusion city scene" << endl;
	cout << "* Press s for make a screenshot" << endl;

	cam.initialize();
	if (init(cam.image_width, cam.image_height) == 1) {
		cout << "App Error! " << std::endl;
		return 1;
	}

	SDL_Event event;
	bool quit = false;

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

			case SDLK_s:
				saveScreenshotBMP("screenshot.bmp");
				break;

			case SDLK_a:
				sphereMultiJittered(cam, world);
				break;

			case SDLK_b:
				sphereRegular(cam, world);
				break;

			case SDLK_c:
				multipleObjects(cam, world);
				break;

			case SDLK_d:
				multipleLights(cam, world);
				break;

			case SDLK_e:
				city(cam, world);
				break;

			}

		}
	}

	close();
	return 0;

}

//int main(int argc, char* argv[])
//{
//	// World
//	hittable_list world;
//
//	//Definisco i colori
//	color red        = color(1.00f, 0.00f, 0.00f);
//	color green      = color(0.00f, 1.00f, 0.00f);
//	color blue       = color(0.00f, 0.00f, 1.00f);
//	color gray       = color(0.50f, 0.50f, 0.50f);
//	color white      = color(1.00f, 1.00f, 1.00f);
//	color black      = color(0.00f, 0.00f, 0.00f);
//	color darkgray   = color(0.25f, 0.25f, 0.25f);
//	color lightgray  = color(0.75f, 0.75f, 0.75f);
//	color slategray  = color(0.40f, 0.50f, 0.56f);
//	color magenta    = color(1.00f, 0.00f, 1.00f);
//	color cyan       = color(0.00f, 1.00f, 1.00f);
//	color brown      = color(0.60f, 0.40f, 0.12f);
//	color yellow     = color(1.00f, 1.00f, 0.00f);
//	color darkyellow = color(0.65f, 0.65f, 0.00f);
//
//	//Sampler
//	int num_samples = 1;
//	multiJittered* sampler_ptr = new multiJittered(num_samples);
//	//regular* sampler_ptr = new regular(num_samples);
//
//	//Luce ambient occluder
//	ambient_occluder* occluder_ptr = new ambient_occluder(white,white,white,1.0);
//	occluder_ptr->set_min_amount(0.0);
//	occluder_ptr->set_sampler(sampler_ptr);
//
//	////Piano
//	//point3 p = point3(0.0f, 0.0f, 0.0f);
//	//vec3 normal = vec3(0.0f, 1.0f, 0.0f);
//	//plane* piano = new plane(p,normal);
//	//texture* colore_piano = new image_texture("../models/grigio.jpg");
//	//material* m_piano = new material(white, white, white, 0.8f, 0.0f);
//	//auto instance_ptr_piano = make_shared<instance>(piano, m_piano);
//	//instance_ptr_piano->translate(0.0f, 0.0f, 0.0f);
//	//m_piano->texture = colore_piano;
//	//world.add(instance_ptr_piano);
//
//	//Cube base
//	mesh* cube = new mesh("../models/cube.obj", "../models/");
//	texture* colore_cubo = new image_texture("../models/grigio.jpg");
//	material* m_cube = new material(white, white, white, 0.8f, 0.0f);
//	auto instance_ptr_cubo = make_shared<instance>(cube, m_cube);
//	instance_ptr_cubo->scale(40.0, 1.0, 40.0);
//	instance_ptr_cubo->translate(0.0f, -0.5f, 0.0f);
//	m_cube->texture = colore_cubo;
//	world.add(instance_ptr_cubo);
//
//	////Sfera base
//	//sphere* sfera_base = new sphere(); //posizione (0,0,0) e raggio 1.0
//	//material* m_sfera_base = new material(gray, gray, white, 1.0f, 0.0f);
//	//auto instace_ptr_sfera_base = make_shared<instance>(sfera_base, m_sfera_base);
//	//texture* colore_sfera_base = new image_texture("../models/grigio.jpg");
//	//instace_ptr_sfera_base->scale(1000.0f, 1000.0f, 1000.0f);
//	//instace_ptr_sfera_base->translate(0.0f, -1000.0f, 0.0f);
//	//m_sfera_base->texture = colore_sfera_base;
//	//world.add(instace_ptr_sfera_base);
//
//	////Sfera principale
//	//sphere* sfera = new sphere(); //posizione (0,0,0) e raggio 1.0
//	//material* m_sfera = new material(yellow, yellow, white, 0.8f, 0.0f);
//	//auto instace_ptr_sfera = make_shared<instance>(sfera, m_sfera);
//	//texture* colore_sfera = new image_texture("../models/giallo.jpg");
//	//instace_ptr_sfera->translate(0.0f, 1.0f, 0.0f);
//	//m_sfera->texture = colore_sfera;
//	//world.add(instace_ptr_sfera);
//
//	//Sfera principale obj
//	mesh* sfera_obj = new mesh("../models/sfera.obj", "../models/");
//	texture* colore_sfera_obj = new image_texture("../models/giallo.jpg");
//	material* m_sfera_obj = new material(yellow, yellow, white, 0.8f, 0.0f);
//	auto instance_ptr_sfera_obj = make_shared<instance>(sfera_obj, m_sfera_obj);
//	instance_ptr_sfera_obj->scale(0.3, 0.3, 0.3);
//	//instance_ptr_sfera_obj->translate(0.0f, -0.5f, 0.0f);
//	m_sfera_obj->texture = colore_sfera_obj;
//	world.add(instance_ptr_sfera_obj);
//
//	////Coniglio 
//	//mesh* bunny = new mesh("../models/bunny.obj", "../models/");
//	//texture* colore_bunny = new image_texture("../models/bianco.jpg");
//	//material* m_bunny = new material(white, white, white, 0.8f, 0.0f);
//	//auto instance_ptr_bunny = make_shared<instance>(bunny, m_bunny);
//	////instance_ptr_bunny->scale(1.0, 1.0, 1.0);
//	////instance_ptr_bunny->translate(0.0f, -0.5f, 0.0f);
//	//m_bunny->texture = colore_bunny;
//	//world.add(instance_ptr_bunny);
//
//	////Gatto 
//	//mesh* cat = new mesh("../models/cat.obj", "../models/");
//	//texture* colore_cat = new image_texture("../models/bianco.jpg");
//	//material* m_cat = new material(blue, blue, white, 0.8f, 0.0f);
//	//auto instance_ptr_cat = make_shared<instance>(cat, m_cat);
//	//instance_ptr_cat->scale(0.05f, 0.05f, 0.05f);
//	//instance_ptr_cat->rotate_y(90.0f);
//	//instance_ptr_cat->rotate_z(90.0f);
//	//instance_ptr_cat->translate(0.0f, 0.0f, 0.0f);
//	//m_cat->texture = colore_cat;
//	//world.add(instance_ptr_cat);
//
//	////Cube principale
//	//texture* colore_cubo2 = new image_texture("../models/giallo.jpg");
//	//material* m_cube_2 = new material(yellow, yellow, white, 0.8f, 0.0f);
//	//auto instance_ptr_cubo_2 = make_shared<instance>(cube, m_cube_2);
//	////instance_ptr_cubo_2->scale(0.5, 0.5, 0.5);
//	//instance_ptr_cubo_2->translate(0.0f, 1.0f, 0.0f);
//	//m_cube_2->texture = colore_cubo2;
//	//world.add(instance_ptr_cubo_2);
//
//
//	//Luce point_light
//	point3 light_position(0.0f, 6.0f, 0.0f);
//	point_light* worldlight = new point_light(light_position, lightgray, lightgray, lightgray);
//	world.add_lights(occluder_ptr);
//	
//	camera cam;
//	cam.lookfrom = point3(0, 5, 10);
//	cam.lookat = point3(0, 0.5, 0);
//
//	cam.aspect_ratio = 16.0f / 9.0f;
//	cam.image_width = 960;
//	//cam.aspect_ratio = 1.0f / 1.0f;
//	//cam.image_width = 400;
//	cam.samples_per_pixel = num_samples;
//	cam.vfov = 20;
//
//	cam.initialize();
//
//
//	if (init(cam.image_width, cam.image_height) == 1) {
//		cout << "App Error! " << std::endl;
//		return 1;
//	}
//
//	cout << "Image Resolution: " << cam.image_width << "x" << cam.image_height << "\n255\n";
//
//	time_t start, end;
//	time(&start);
//
//	//cam.parallel_render(world, *worldlight);
//	cam.parallel_render_ambient_occlusion(world, *occluder_ptr); //render con occlusione ambientale
//	//cam.render(world, *worldlight);
//
//	time(&end);
//	double dif = difftime(end, start);
//	cout << "\n" << "Rendering time: " << dif << "\n";
//
//	SDL_Event event;
//	bool quit = false;
//
//	/* Poll for events */
//	while (SDL_PollEvent(&event) || (!quit)) {
//
//		switch (event.type) {
//
//		case SDL_QUIT:
//			quit = true;
//			break;
//
//		case SDL_KEYDOWN:
//			switch (event.key.keysym.sym) {
//			case SDLK_ESCAPE:
//				quit = true;
//				break;
//				// cases for other keypresses
//
//			case SDLK_s:
//				saveScreenshotBMP("screenshot.bmp");
//				cout << "Screenshot saved!" << endl;
//				break;
//			}
//		}
//	}
//
//	close();
//	return 0;
//}
