#include "sampler.h"

class pure_random : public sampler {
public:

    pure_random() = delete;

    explicit pure_random(const int num);

    ~pure_random() = default;

    pure_random(const pure_random& r) = default;

    pure_random(pure_random&& r) = default;

    pure_random& operator=(const pure_random& rhs) = default;

    pure_random& operator=(pure_random&& rhs) = default;

    virtual pure_random* clone() const override;

private:

    virtual void generate_samples() override;
};

pure_random::pure_random(const int num) : sampler(num) { generate_samples(); }

pure_random* pure_random::clone() const { return new pure_random(*this); }

void pure_random::generate_samples() {
    for (int p = 0; p < num_sets; p++) {
        for (int q = 0; q < num_samples; q++) {
            samples.push_back(point2(rand_float(), rand_float()));
        }
    }
}