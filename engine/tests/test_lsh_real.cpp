#include "lsh.h"
#include "loader.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <queue>
#include <algorithm>

// LSH benchmark on the real dataset


// Claudio's brute force function to test baseline
// max heap of the k best neighbors, worst on top
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

    const int k = 5;
    int num_queries = 1000;

    std::vector<int> query_rows(num_queries);

    for (int i = 0; i < num_queries; i++) {
        query_rows[i] = i;  }      // same queries for consistent results
    std::cout << "Queries: rows 0 to " << query_rows[num_queries - 1] << std::endl;

    // brute force answer key

    std::vector<std::vector<int>> truth(num_queries);
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < num_queries; i++) {
        const float* q = &data[query_rows[i] * dim];
        truth[i] = bruteForce(data.data(), n, dim, q, k + 1);
    }
    auto end = std::chrono::steady_clock::now();

    double bfMs = std::chrono::duration<double, std::milli>(end - start).count() / num_queries;
    std::cout << "Brute force: " << bfMs << " ms per query" << std::endl;

    std::ofstream csv("benchmarks/results/lsh.csv");
    if (!csv) {
        std::cerr << "Failed to open CSV file for writing\n";
        return 1;
    }
    csv << "method,bits,tables,share_checked,recall_top6,avg_checked,ms_per_query,build_ms\n";
    csv << "brute_force,all,all,1.000,1.000000," << n << "," << bfMs << ",0\n";



    // LSH benchmark + parameter tuning
    // fewer bits means bigger buckets, more checked so better recall
    // more tables means more chances to find a neighbor, better recall but more checked

    int bitsList[] = {8, 10, 12, 16};
    int tablesList[] = {10, 20};

    for (int bits : bitsList) {
        for (int tables : tablesList) {
            LSH lsh(bits, tables);
            auto build_start = std::chrono::steady_clock::now();
            lsh.build(data.data(), n, dim);
            auto build_end = std::chrono::steady_clock::now();
            double build_ms = std::chrono::duration<double, std::milli>(build_end - build_start).count();

            long long found = 0;
            long long checked_sum = 0;

            start = std::chrono::steady_clock::now();
            for (int i = 0; i < num_queries; i++) {
                const float* q = &data[query_rows[i] * dim];
                auto lsh_res = lsh.query(q, k + 1);
                checked_sum += lsh.last_checked();

                // compare to brute force truth
                for (int t : truth[i]) {
                    for (auto& g : lsh_res) {
                        if (g.first == t) {
                            found++;
                            break;
                        }
                    }
                }

            }
            end = std::chrono::steady_clock::now();


            double ms = std::chrono::duration<double, std::milli>(end - start).count() / num_queries;
            double recall = (double)found / (num_queries * (k + 1));
            long long avg_checked = checked_sum / num_queries;
            double share_checked = (double)avg_checked / n;

            std::cout << "bits=" << bits << " tables=" << tables << " recall=" << recall << " checked=" << avg_checked << " " << ms << " ms" << " (build " << build_ms / 1000.0 << " s)\n";

            csv << "lsh," << bits << "," << tables << "," << share_checked << "," << recall << "," << avg_checked << "," << ms << "," << build_ms << "\n";
        }
    }


    std::cout << "wrote benchmarks/results/lsh.csv\n";
}