#include "lsh.h"
#include <algorithm>
#include <random>


LSH::LSH(int bits, int tables, unsigned int seed)
    : bits_(bits), tables_(tables), seed_(seed) {}


// helper functions

// dot product of two vectors -> cosine similarity functionality
float dot(const std::vector<float>& a, const std::vector<float>& b) {
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}



// fill arrows_ with random hyperplanes for hashing
void random_arrows() {
    std::mt19937 rng(seed_);
    std::normal_distribution<float> dist(0.0f, 1.0f);

    arrows_.resize((size_t)tables_ * bits_ * dim_);
    for (float& arrow : arrows_) {
        arrow = dist(rng);
    }
}