#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <cstdint>



// Locality-sensitive hashing (LSH) for approximate nearest neighbor search
// using cosine similarity
//
// This implementation uses random hyperplanes ("arrows") to hash vectors into buckets
// based on their cosine similarity. An inputted movie gets a 1 for an arrow
// if it points the same direction as the arrow, and a 0 if it points in the
// opposite. Similar movies get the similar codes. We will build multiple
// tables to reduce the probability of false negatives.


class LSH {
public:
    LSH(int bits = 10, int tables = 20, unsigned int seed = 21);

    //note: bits can't go above 32, because we are using a 32-bit integer to store the hash code for speed

    // n vectors of dimension dim -> data
    void build(const float* data, int n, int dim);

    // query the LSH for the k nearest neighbors of the query vector
    // returns a list of k best vectors, with (row number, score) as a pair
    std::vector<std::pair<int, float>> query(const float* q, int k) const;

    int last_checked() const { return last_checked_; } // for benchmarking, how many movies were checked in the last query

private:
    // settings and seed
    int bits_;
    int num_tables_;
    unsigned int seed_;

    // random arrows for hashing
    std::vector<float> arrows_;

    // movie data and dimensions
    const float* data_ = nullptr;
    int n_ = 0;
    int dim_ = 0;

    // hash tables for LSH
    using Table = std::unordered_map<uint32_t, std::vector<int>>;
    std::vector<Table> tables_;

    mutable int last_checked_ = 0; // mutable because we want to update it in a const function

    // helper functions inside LSH class
    float dot(const float* a, const float* b) const;

    void make_random_arrows();
    const float* get_arrow(int table, int bit) const;


    uint32_t hash(const float* vec, int table) const;
};