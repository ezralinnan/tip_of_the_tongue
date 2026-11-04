#include "kd_tree.h"
#include <iostream>

int main() {
    // A(1,2) B(2,6) C(3,9) D(4,4) E(6,1) F(7,7) G(8,3) H(9,8)  — rows 0..7
    float pts[] = {1,2,  2,6,  3,9,  4,4,  6,1,  7,7,  8,3,  9,8}; // array of coordinates, this is super helpful to understand how vectorization works
    // basically it is a flat array of 384 coordinates per movie, and they're back to back. I just used the spaces for cleaner visual
    KDTree tree(2);
    tree.build(pts, 8, 2);
    tree.print();

    float q[] = {3, 4};
    auto res = tree.query(q, 2);
    for (auto& p : res) std::cout << "row " << p.first << "\n";
}