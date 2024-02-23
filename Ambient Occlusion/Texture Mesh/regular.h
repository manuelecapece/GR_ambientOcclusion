#pragma once

#include "sampler.h"
#include "point2.h"

class regular : public sampler {
public:

    regular() = delete;

    explicit regular(const int num);

    ~regular() = default;

    regular(const regular& r) = default;

    regular(regular&& r) = default;

    regular& operator=(const regular& rhs) = default;

    regular& operator=(regular&& rhs) = default;

    virtual regular* clone() const override;

private:

    virtual void generate_samples() override;
};

#include "regular.h"

regular::regular(const int num) : sampler(num) { generate_samples(); }

regular* regular::clone() const { return new regular(*this); }

void regular::generate_samples() {
    int n = (int)sqrt((float)num_samples);

    for (int j = 0; j < num_sets; j++) {
        for (int p = 0; p < n; p++) {
            for (int q = 0; q < n; q++) {
                samples.push_back(point2(((float)q + 0.5f) / (float)n, ((float)p + 0.5f) / (float)n));
            }
        }
    }
}