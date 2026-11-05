#include "lsh.h"
#include <algorithm>
#include <random>


LSH::LSH(int bits, int tables, unsigned int seed)
    : bits_(bits), num_tables_(tables), seed_(seed) {}


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

    arrows_.resize((size_t)num_tables_ * bits_ * dim_);
    for (float& arrow : arrows_) {
        arrow = bell(rng);
    }
}

// get the pointer to the arrow for a specific table and bit
const float* LSH::get_arrow(int table, int bit) const {
    return &arrows_[((size_t)table * bits_ + bit) * dim_];
}


// hash a vector into a 32-bit integer code for a specific table
// each arrow contributes one bit, 1 for same direction, 0 for opposite
uint32_t LSH::hash(const float* vec, int table) const {
    uint32_t code = 0;
    for (int bit = 0; bit < bits_; bit++) {
        bool same_direction = dot(vec, get_arrow(table, bit)) >= 0; // 1/0 for each arrow based on dot product
        code = (code << 1) | (same_direction ? 1 : 0); // add bit onto the code, shifting left
    }
    return code;
}


// build function: takes in n vectors of dimension dim and stores them in the LSH structure

void LSH::build(const float* data, int n, int dim) {
    data_ = data;
    n_ = n;
    dim_ = dim;

    make_random_arrows(); // generate random hyperplanes

    // put every movie into a bucket in every table

    tables_.assign(num_tables_, Table()); // initialize hash tables
    for (int row = 0; row < n_; ++row) {
        for (int table = 0; table < num_tables_; table++) {
            uint32_t code = hash(&data_[row * dim_], table); // hash each vector
            tables_[table][code].push_back(row); // store the index in the corresponding bucket
        }
    }
} // runs once and then querying is fast



// query function: takes in a query vector and returns the k nearest neighbors based on cosine similarity

std::vector<std::pair<int, float>> LSH::query(const float* q, int k) const {
    std::vector<int> candidates;

    for (int table = 0; table < num_tables_; table++) {
        uint32_t code = hash(q, table); // hash the query vector
        
        auto bucket = tables_[table].find(code); // find the bucket for this hash code
        
        if (bucket != tables_[table].end()) {
            candidates.insert(candidates.end(), bucket->second.begin(), bucket->second.end()); // add all candidates from this bucket
        }
    }

    std::sort(candidates.begin(), candidates.end());
    candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

    std::vector<std::pair<int, float>> scores;
    for (int idx : candidates) {
        float score = dot(q, &data_[idx * dim_]); // compute cosine similarity
        scores.emplace_back(idx, score);
    }

    int keep = std::min<int>(k, (int)scores.size());
    scores.resize(keep);
    return scores;
}


