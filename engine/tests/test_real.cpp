#include "kd_tree.h"
#include "loader.h"
#include <iostream>
#include <chrono>
#include <algorithm>


std::vector<int> bruteForce(const float* data, int n, int dim, const float* q, int k) {

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
    std::vector<int> rows;
    while (!heap.empty()) { rows.push_back(heap.top().second); heap.pop(); }
    std::reverse(rows.begin(), rows.end());               // best first
    return rows;
}

int main() {
    std::vector<float> data;
    int n = 0, dim = 0;
    if (!loadVectors("data/vectors.bin", data, n, dim)) return 1;
    std::cout << "vectors: " << n << " x " << dim << "\n";

    const int k = 5, numQueries = 1000;

    std::vector<int> queryRows(numQueries);
    for (int i = 0; i < numQueries; i++) {
        queryRows[i] = i;  }      // most popular movies approach

    std::cout << "queries: rows 0 to " << queryRows[numQueries - 1] << "\n";

    // test brute force on The Dark Knight (my favorite movie, in row 2)
    std::vector<int> test = bruteForce(data.data(), n, dim, &data[2 * dim], k + 1);
    std::cout << "brute force for row 2:";
    for (int r : test) std::cout << " " << r;
    std::cout << "\n";

    KDTree tree;
    tree.build(data.data(), n, dim);

    std::cout << "leaves: " << tree.leafCount() << "\n";

    auto kd = tree.query(&data[2 * dim], k + 1);
    std::cout << "kd-tree for row 2:    ";
    for (auto& p : kd) std::cout << " " << p.first;        // p.first = row number
    std::cout << "\n";

    // compare row by row
    bool same = (kd.size() == test.size());
    for (size_t j = 0; same && j < kd.size(); j++) {
        if (kd[j].first != test[j]) same = false;}

    
    std::vector<std::vector<int>> truth(numQueries);

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < numQueries; i++) {
        const float* q = &data[queryRows[i] * dim];
        truth[i] = bruteForce(data.data(), n, dim, q, k + 1);
    }
    auto end = std::chrono::steady_clock::now();

    double bfMs = std::chrono::duration<double, std::milli>(end - start).count() / numQueries;
    std::cout << "brute force: " << bfMs << " ms per query\n";

    // now compare to exact KD tree
    int matches = 0;
    long long checkedSum = 0;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < numQueries; i++) {
        const float* q = &data[queryRows[i] * dim];
        auto kdRes = tree.query(q, k + 1);
        checkedSum += tree.lastChecked();

        bool same = (kdRes.size() == truth[i].size());
        for (size_t j = 0; same && j < kdRes.size(); j++) {
            if (kdRes[j].first != truth[i][j]) same = false;
        }
        if (same) matches++;
    }
    end = std::chrono::steady_clock::now();

    double kdMs = std::chrono::duration<double, std::milli>(end - start).count() / numQueries;
    std::cout << "exact kd-tree: " << matches << "/" << numQueries << " match, " << checkedSum / numQueries << " checked, " << kdMs << " ms per query\n";

    int caps[] = {82, 164, 410, 819, 2048, tree.leafCount()}; // the custom capped leaves

    // we need some variables, such as found, to be initiated and restarted every time here
    for (int m : caps) {
        tree.setMaxLeaves(m); // make sure we go down the other path, searchCapped()
        long long found = 0, capChecked = 0;

        start = std::chrono::steady_clock::now(); // clock of course
        for (int i = 0; i < numQueries; i++) { // our 1000 queries
            const float* q = &data[queryRows[i] * dim];
            auto kdRes = tree.query(q, k + 1); // more than one k (not the exact neighbor)
            capChecked += tree.lastChecked();

            for (int t : truth[i])                       // each true neighbor
                for (auto& g : kdRes)                    // did the KD-tree find it?
                    if (g.first == t) {
                        found++; // yes, we found it 
                        break; // dont check anymore, od 
                    }
        }
        end = std::chrono::steady_clock::now();

        double capMs = std::chrono::duration<double, std::milli>(end - start).count() / numQueries;
        std::cout << "m=" << m
                  << "  recall=" << (double)found / ((k + 1) * numQueries)
                  << "  checked=" << capChecked / numQueries
                  << "  " << capMs << " ms\n";
    }
    tree.setMaxLeaves(-1);  

}