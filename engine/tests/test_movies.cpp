#include "loader.h"
#include "kd_tree.h"
#include <iostream>

int main(){
    std::vector<float> data;
    int n = 0, dim = 0;
    if (!loadVectors("data/vectors.bin", data, n, dim)) return 1; // use my function just written

    std::vector<Movie> movies; // using movie objects
    if (!loadMovies("data/movies.csv", movies)) return 1; // use this to compare against

    std::cout << "vectors: " << n << " x " << dim << std::endl;
    std::cout << "movies:  " << movies.size() << std::endl;
    if ((int)movies.size() != n) {
        std::cout << "files don't line up\n";
        return 1;
    }

    // 2. build the tree on the real vectors
    KDTree tree;
    tree.build(data.data(), n, dim);
    std::cout << "leaves:  " << tree.leafCount() << std::endl;

    // 3. Inception (row 0) as its own query: print the 6 nearest titles
    const float* q = &data[2 * dim]; // query here
    auto res = tree.query(q, 6); // 6 nearest neighbors
    for (auto& p : res)
        std::cout << movies[p.first].title << " (" << movies[p.first].year << ")"
                    << "  score=" << p.second << std::endl;
    std::cout << "checked: " << tree.lastChecked() << " of " << n << std::endl;
    }