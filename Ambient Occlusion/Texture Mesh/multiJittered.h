

#include "sampler.h"
#include "point2.h"
#include "common.h"


class multiJittered : public sampler {
public:

    multiJittered() = default;

    explicit multiJittered(const int num_samples);

    explicit multiJittered(const int num_samples, const int m);

    ~multiJittered() = default;

    multiJittered(const multiJittered& r) = default;

    multiJittered(multiJittered&& r) = default;

    multiJittered& operator=(const multiJittered& rhs) = default;

    multiJittered& operator=(multiJittered&& rhs) = default;

    virtual multiJittered* clone() const override;

private:

    virtual void generate_samples() override;
};

multiJittered::multiJittered(const int num_samples) : sampler(num_samples) { generate_samples(); }

multiJittered::multiJittered(const int num_samples, const int m) : sampler(num_samples, m) { generate_samples(); }

multiJittered* multiJittered::clone() const { return new multiJittered(*this); }


void multiJittered::generate_samples() {
    // num_samples needs to be a perfect square

    int n = (int)sqrt((float)num_samples);
    float subcell_width = 1.0f / ((float)num_samples);

    // fill the samples array with dummy points to allow us to use the [ ] notation when we set the
    // initial patterns

    point2 fill_point;
    for (int j = 0; j < num_samples * num_sets; j++) {
        samples.push_back(fill_point);
    }

    // distribute points in the initial patterns

    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                samples[i * n + j + p * num_samples].x = (i * n + j) * subcell_width + rand_float(0, subcell_width);
                samples[i * n + j + p * num_samples].y = (j * n + i) * subcell_width + rand_float(0, subcell_width);
            }
        }
    }

    // shuffle x coordinates

    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int k = rand_int(j, n - 1);
                float t = samples[i * n + j + p * num_samples].x;
                samples[i * n + j + p * num_samples].x = samples[i * n + k + p * num_samples].x;
                samples[i * n + k + p * num_samples].x = t;
            }
        }
    }

    // shuffle y coordinates

    for (int p = 0; p < num_sets; p++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int k = rand_int(j, n - 1);
                float t = samples[j * n + i + p * num_samples].y;
                samples[j * n + i + p * num_samples].y = samples[k * n + i + p * num_samples].y;
                samples[k * n + i + p * num_samples].y = t;
            }
        }
    }
}