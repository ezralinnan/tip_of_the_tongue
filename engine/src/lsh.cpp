#include "lsh.h"
#include <algorithm>
#include <random>


float dot(const std::vector<float>& a, const std::vector<float>& b) {
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

void random_arrows() {
    std::mt19937 rng(21);
    std::normal_distribution<float> dist(0.0f, 1.0f);

}