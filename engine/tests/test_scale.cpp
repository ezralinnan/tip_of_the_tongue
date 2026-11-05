#include "kd_tree.h"
#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>

std::vector<float> randomUnitVectors(int n, int dim, unsigned seed) { // return a vector of floats
    std::mt19937 rng(seed);                              // random generator with fixed seed, from C++ doc
    std::normal_distribution<float> gauss(0.0f, 1.0f);   // bell curve, mean 0, std 1 GAUSSIAN

    std::vector<float> out(n * dim); // out is a vector of size n * dim
    for (int r = 0; r < n; r++) { // simple loop better
        float lenSq = 0.0f;
        for (int j = 0; j < dim; j++) {
            float x = gauss(rng); // what is x
            out[r * dim + j] = x; // this is assigned in the correct spot, same structure as my KD tree
            lenSq += x * x;
        }
        float len = std::sqrt(lenSq); // take sqrt (dealing with negatives, the length itself is sqrt of lSq)
        for (int j = 0; j < dim; j++) out[r * dim + j] /= len;   // scale to length 1, simplest for loop
    }
    return out; // the vector, random seed
}

std::vector<std::pair<int, float>> bruteForce(const float* data, int n, int dim,
                                              const float* q, int k) {

    // im gonna be using the EXACT same 3 step heap method i used in KD Tree, now brute force does it for every one.
    std::priority_queue<std::pair<float, int>> heap;     // (distsq, row), worst on top
    for (int r = 0; r < n; r++) {  // number of movies and all
        float dist = 0.0f;
        for (int j = 0; j < dim; j++) { // dimension counter
            float diff = q[j] - data[r * dim + j];
            dist += diff * diff;
        }
        if ((int)heap.size() < k) {
            heap.push({dist, r});
        } else if (dist < heap.top().first) { 
            heap.pop(); 
            heap.push({dist, r});
        }
    }
    std::vector<std::pair<int, float>> result;
    while (!heap.empty()) {
        result.push_back({heap.top().second, 1.0f - heap.top().first / 2.0f}); // using scores, not distances
        heap.pop();
    }
    std::reverse(result.begin(), result.end());          // just like last time
    return result; //
}

int main() {
    const int n = 10000, dim = 384, k = 5, numQueries = 100;

    std::vector<float> data    = randomUnitVectors(n, dim, 42);           // the films
    std::vector<float> queries = randomUnitVectors(numQueries, dim, 7);   // different seed, 7, to test for difference like in actual problem

    KDTree tree;                          // leaf size 16 when not initiated otherwise
    tree.build(data.data(), n, dim);      // pointer strategy from earlier

    int matches = 0;
    for (int i = 0; i < numQueries; i++) {
        const float* q = &queries[i * dim];                  // query i starts here
        auto kd = tree.query(q, k);
        auto bf = bruteForce(data.data(), n, dim, q, k);

        bool same = (kd.size() == bf.size());
        for (size_t j = 0; same && j < kd.size(); j++)
            if (kd[j].first != bf[j].first) same = false;     // compare row numbers in order

        if (same) matches++;
        else std::cout << "not the same on query number" << i << "\n";
    }
    std::cout << matches << " out of " << numQueries << " match\n";
}