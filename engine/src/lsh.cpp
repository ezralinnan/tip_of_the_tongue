#include "lsh.h"
#include <algorithm>
#include <random>


LSH::LSH(int bits, int tables, unsigned int seed)
    : bits_(bits), tables_(tables), seed_(seed) {}


// helper functions

// dot product of two lists -> cosine similarity functionality
float LSH::dot(const float* a, const float* b) const {
    float result = 0.0f;
    for (int i = 0; i < dim_; ++i) {
        result += a[i] * b[i];
    }
    return result;
}



// fill arrows_ with random hyperplanes for hashing
void LSH::make_random_arrows() {
    std::mt19937 rng(seed_);
    std::normal_distribution<float> bell(0.0f, 1.0f); // bell curve distribution

    arrows_.resize(tables_ * bits_ * dim_);
    for (float& arrow : arrows_) {
        arrow = bell(rng);
    }
}

// get the pointer to the arrow for a specific table and bit
const float* LSH::get_arrow(int table, int bit) const {
    return &arrows_[(table * bits_ + bit) * dim_];
}


// hash a vector into a 32-bit integer code for a specific table
// each arrow contributes one bit, 1 for same direction, 0 for opposite
uint32_t LSH::hash(const float* vector, int table) const {
    uint32_t code = 0;
    for (int bit = 0; bit < bits_; bit++) {
        bool same_direction = dot(vector, get_arrow(table, bit)) >= 0; // 1/0 for each arrow based on dot product
        code = (code << 1) | (same_direction ? 1 : 0); // add bit onto the code, shifting left
    }
    return code;
}