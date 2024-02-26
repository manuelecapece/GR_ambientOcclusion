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

    sampler(const int num);

    virtual sampler* clone() const = 0;

    virtual void generate_samples() = 0;// generate sample patterns in a unit square

    int get_num_samples();

    void shuffle_x_coordinates();

    void shuffle_y_coordinates();

    void setup_shuffled_indices();

    void map_samples_to_hemisphere(const float p);

    point3 sample_hemisphere(); // get next sample on unit hemisphere

protected:

    int num_samples;                          // the number of sample points in a set
    int num_sets;                             // the number of sample sets
    std::vector<point2> samples;              // sample points on a unit square
    std::vector<int> shuffled_indices;        // shuffled samples array indices
    std::vector<point3> hemisphere_samples;   // sample points on a unit hemisphere
    unsigned long count;                      // the current number of sample points used
    int jump;                                 // random index jump
};


sampler::sampler(const int ns) : num_samples(ns), num_sets(83), count(0), jump(0) {
    samples.reserve(num_samples * num_sets);
    setup_shuffled_indices();
}

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

point3 sampler::sample_hemisphere() {
    if (count % num_samples == 0) {  // start of a new pixel
        jump = (rand_int() % num_sets) * num_samples;
    }

    return hemisphere_samples[jump + shuffled_indices[jump + count++ % num_samples]];
}


