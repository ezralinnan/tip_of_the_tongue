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
        return 1;
    }

    if (!loadVectors("data/vectors.bin", vectors, n, dim)) {
        return 1;
    }

    std::vector<float> query(dim);

    std::ifstream file("data/query_vector.bin", std::ios::binary);

    if (!file) {
        return 1;
    }

    file.read(
        reinterpret_cast<char*>(query.data()),
        dim * sizeof(float)
    );

    // ---------- KD TREE ----------

    KDTree tree;
    tree.build(vectors.data(), n, dim);

    auto kdResults = tree.query(query.data(), 5);

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
    // Runs in the background for comparison page

    LSH lsh(10, 20);

    lsh.build(vectors.data(), n, dim);

    auto lshResults = lsh.query(query.data(), 5);

    return 0;
}