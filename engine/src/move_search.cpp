#include "kd_tree.h"
#include "lsh.h"
#include "loader.h"

#include <iostream>
#include <fstream>
#include <vector>

int main() {

    std::vector<Movie> movies;
    std::vector<float> vectors;

    int n = 0;
    int dim = 0;

    if (!loadMovies("data/movies.csv", movies)) {
        std::cout << "Failed to load movies\n";
        return 1;
    }

    if (!loadVectors("data/vectors.bin", vectors, n, dim)) {
        std::cout << "Failed to load vectors\n";
        return 1;
    }

    std::vector<float> query(dim);

    std::ifstream file("data/query_vector.bin", std::ios::binary);

    if (!file) {
        std::cout << "Could not open query_vector.bin\n";
        return 1;
    }

    file.read(
        reinterpret_cast<char*>(query.data()),
        dim * sizeof(float)
    );

    std::cout << "Loaded query vector\n";
    std::cout << "Loaded " << movies.size() << " movies\n";

    return 0;
}