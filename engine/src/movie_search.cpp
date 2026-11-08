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

// ---------- KD TREE ----------

KDTree tree;
tree.build(vectors.data(), n, dim);

auto kdResults = tree.query(query.data(), 5);

std::cout << "\nTop KD-Tree Results:\n\n";

for (auto& result : kdResults) {

    int row = result.first;
    float score = result.second;

    if (row >= 0 && row < (int)movies.size()) {

        std::cout
            << movies[row].title
            << " ("
            << movies[row].year
            << ")"
            << "  Score: "
            << score
            << "\n";
    }
}


// ---------- LSH ----------

LSH lsh(10, 20);

lsh.build(vectors.data(), n, dim);

auto lshResults = lsh.query(query.data(), 5);

std::cout << "\nTop LSH Results:\n\n";

for (auto& result : lshResults) {

    int row = result.first;
    float score = result.second;

    if (row >= 0 && row < (int)movies.size()) {

        std::cout
            << movies[row].title
            << " ("
            << movies[row].year
            << ")"
            << "  Score: "
            << score
            << "\n";
    }
}

    return 0;
}