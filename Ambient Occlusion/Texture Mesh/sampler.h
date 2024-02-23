#pragma once

#include <vector>
#include <cmath>
#include "point2.h"
#include "vec3.h"
#include "common.h"
#include <math.h>
#include <algorithm>  // for random_shuffle in Sampler::setup_shuffled_indices
#include <iostream>
#include <random>

class sampler {
public:

    sampler();

    sampler(const int num);

    sampler(const int num, const int num_sets);

    virtual ~sampler();

    sampler(const sampler& s);

    sampler(sampler&& s) noexcept;

    sampler& operator=(const sampler& s);

    sampler& operator=(sampler&& s) noexcept;

    virtual sampler* clone() const = 0;

    void set_num_sets(const int np);

    virtual void  // generate sample patterns in a unit square
        generate_samples() = 0;

    int get_num_samples();

    void shuffle_x_coordinates();

    void shuffle_y_coordinates();

    void setup_shuffled_indices();

    void map_samples_to_unit_disk();

    void map_samples_to_hemisphere(const float p);

    void map_samples_to_sphere();

    // the following functions are not const because they change count and jump

    point2  // get next sample on unit square
        sample_unit_square();

    point2  // get next sample on unit disk
        sample_unit_disk();

    point3  // get next sample on unit hemisphere
        sample_hemisphere();

    point3  // get next sample on unit sphere
        sample_sphere();

    point2            // only used to set up a vector noise table
        sample_one_set();  // this is not discussed in the book, but see the
    // file LatticeNoise.cpp in Chapter 31

protected:

    int num_samples;                          // the number of sample points in a set
    int num_sets;                             // the number of sample sets
    std::vector<point2> samples;             // sample points on a unit square
    std::vector<int> shuffled_indices;        // shuffled samples array indices
    std::vector<point2> disk_samples;        // sample points on a unit disk
    std::vector<point3> hemisphere_samples;  // sample points on a unit hemisphere
    std::vector<point3> sphere_samples;      // sample points on a unit sphere
    unsigned long count;                      // the current number of sample points used
    int jump;                                 // random index jump
};


sampler::sampler() : num_samples(1), num_sets(83), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

sampler::sampler(const int ns) : num_samples(ns), num_sets(83), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

sampler::sampler(const int ns, const int n_sets) : num_samples(ns), num_sets(n_sets), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

sampler::~sampler() {}

sampler::sampler(const sampler& s)
    : num_samples(s.num_samples),
    num_sets(s.num_sets),
    samples(s.samples),
    shuffled_indices(s.shuffled_indices),
    disk_samples(s.disk_samples),
    hemisphere_samples(s.hemisphere_samples),
    sphere_samples(s.sphere_samples),
    count(s.count),
    jump(s.jump) {}

sampler::sampler(sampler&& s) noexcept
    : num_samples(std::exchange(s.num_samples, 1)),
    num_sets(std::exchange(s.num_sets, 83)),
    samples(std::move(s.samples)),
    shuffled_indices(std::move(s.shuffled_indices)),
    disk_samples(std::move(s.disk_samples)),
    hemisphere_samples(std::move(s.hemisphere_samples)),
    sphere_samples(std::move(s.sphere_samples)),
    count(std::exchange(s.count, 0)),
    jump(std::exchange(s.jump, 0)) {}

sampler& sampler::operator=(const sampler& s) {
    num_samples = s.num_samples;
    num_sets = s.num_sets;
    samples = s.samples;
    shuffled_indices = s.shuffled_indices;
    disk_samples = s.disk_samples;
    hemisphere_samples = s.hemisphere_samples;
    sphere_samples = s.sphere_samples;
    count = s.count;
    jump = s.jump;

    return *this;
}

sampler& sampler::operator=(sampler&& s) noexcept {
    num_samples = std::exchange(s.num_samples, 1);
    num_sets = std::exchange(s.num_sets, 83);
    samples = std::move(s.samples);
    shuffled_indices = std::move(s.shuffled_indices);
    disk_samples = std::move(s.disk_samples);
    hemisphere_samples = std::move(s.hemisphere_samples);
    sphere_samples = std::move(s.sphere_samples);
    count = std::exchange(s.count, 0);
    jump = std::exchange(s.jump, 0);

    return *this;
}

void sampler::set_num_sets(const int np) { num_sets = np; }

int sampler::get_num_samples() { return num_samples; }

// shuffle the x coordinates of the points within each set

void sampler::shuffle_x_coordinates() {
    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < num_samples - 1; i++) {
            int target = rand_int() % num_samples + p * num_samples;
            float temp = samples[i + p * num_samples + 1].x;
            samples[i + p * num_samples + 1].x = samples[target].x;
            samples[target].x = temp;
        }
    }
}

// shuffle the y coordinates of the points within set

void sampler::shuffle_y_coordinates() {
    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < num_samples - 1; i++) {
            int target = rand_int() % num_samples + p * num_samples;
            float temp = samples[i + p * num_samples + 1].y;
            samples[i + p * num_samples + 1].y = samples[target].y;
            samples[target].y = temp;
        }
    }
}

// sets up randomly shuffled indices for the samples array

void sampler::setup_shuffled_indices() {
    shuffled_indices.reserve(num_samples * num_sets);
    std::vector<int> indices;
    std::random_device rd;
    std::mt19937 g(rd());

    for (int j = 0; j < num_samples; j++) {
        indices.push_back(j);
    }

    for (int p = 0; p < num_sets; p++) {
        std::shuffle(indices.begin(), indices.end(), g);

        for (int j = 0; j < num_samples; j++) {
            shuffled_indices.push_back(indices[j]);
        }
    }
}

// Maps the 2D sample points in the square [-1,1] X [-1,1] to a unit disk, using Peter Shirley's
// concentric map function

void sampler::map_samples_to_unit_disk() {
    size_t size = samples.size();
    float r, phi;  // polar coordinates
    point2 sp;    // sample point on unit disk

    disk_samples.reserve(size);

    for (int j = 0; j < size; j++) {
        // map sample point to [-1, 1] X [-1,1]

        sp.x = 2.0f * samples[j].x - 1.0f;
        sp.y = 2.0f * samples[j].y - 1.0f;

        if (sp.x > -sp.y) {     // sectors 1 and 2
            if (sp.x > sp.y) {  // sector 1
                r = sp.x;
                phi = sp.y / sp.x;
            }
            else {  // sector 2
                r = sp.y;
                phi = 2 - sp.x / sp.y;
            }
        }
        else {                // sectors 3 and 4
            if (sp.x < sp.y) {  // sector 3
                r = -sp.x;
                phi = 4 + sp.y / sp.x;
            }
            else {  // sector 4
                r = -sp.y;
                if (sp.y != 0.0f) {  // avoid division by zero at origin
                    phi = 6 - sp.x / sp.y;
                }
                else {
                    phi = 0.0f;
                }
            }
        }

        phi *= pi / 4.0f;

        disk_samples[j].x = r * (float)cos(phi);
        disk_samples[j].y = r * (float)sin(phi);
    }

    samples.erase(samples.begin(), samples.end());
}

// Maps the 2D sample points to 3D points on a unit hemisphere with a cosine power
// density distribution in the polar angle

void sampler::map_samples_to_hemisphere(const float exp) {
    size_t size = samples.size();
    hemisphere_samples.reserve(num_samples * num_sets);

    for (int j = 0; j < size; j++) {
        float cos_phi = cos(2.0 * pi * samples[j].x);
        float sin_phi = sin(2.0 * pi * samples[j].x);
        float cos_theta = pow((1.0f - samples[j].y), 1.0f / (exp + 1.0f));
        float sin_theta = sqrt(1.0f - cos_theta * cos_theta);
        float pu = sin_theta * cos_phi;
        float pv = sin_theta * sin_phi;
        float pw = cos_theta;
        hemisphere_samples.push_back(point3((float)pu, (float)pv, (float)pw));
    }
}

// Maps the 2D sample points to 3D points on a unit sphere with a uniform density
// distribution over the surface
// this is used for modelling a spherical light

void sampler::map_samples_to_sphere() {
    sphere_samples.reserve(num_samples * num_sets);

    for (int j = 0; j < num_samples * num_sets; j++) {
        float r1 = samples[j].x;
        float r2 = samples[j].y;
        float z = 1.0f - 2.0f * r1;
        float r = (float)sqrt(1.0f - z * z);
        float phi = TWO_PI * r2;
        float x = r * (float)cos(phi);
        float y = r * (float)sin(phi);
        sphere_samples.push_back(point3(x, y, z));
    }
}

// the final version in Listing 5.13

point2 sampler::sample_unit_square() {
    if (count % num_samples == 0) {                    // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;  // random index jump initialised to zero in constructor
    }

    return samples[jump + shuffled_indices[jump + count++ % num_samples]];
}

point2 sampler::sample_unit_disk() {
    if (count % num_samples == 0) {  // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;
    }

    return disk_samples[jump + shuffled_indices[jump + count++ % num_samples]];
}

point3 sampler::sample_hemisphere() {
    if (count % num_samples == 0) {  // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;
    }

    return hemisphere_samples[jump + shuffled_indices[jump + count++ % num_samples]];
}

point3 sampler::sample_sphere() {
    if (count % num_samples == 0) {  // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;
    }

    return sphere_samples[jump + shuffled_indices[jump + count++ % num_samples]];
}

// This is a specialised function called in LatticeNoise::init_vector_table
// It doesn't shuffle the indices

point2 sampler::sample_one_set() { return samples[count++ % num_samples]; }