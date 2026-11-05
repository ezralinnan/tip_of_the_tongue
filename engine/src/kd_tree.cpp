#include "kd_tree.h"
#include <algorithm>
#include <iostream>
#include <string>    

void KDTree::build(const float* data, int n, int dim){
    destroy(root);
    this->data = data;
    this->nMovies = n;
    this->dim  = dim;

    idx.resize(n);
    for (int i = 0; i < this->nMovies; i++){
        idx[i] = i;
    }

    numLeaves = 0;
    root = buildRec(0, n, 0);
    return;
}

Node* KDTree::buildRec(int lo, int hi, int depth){
    Node* node = new Node;

    // base case size < max leafSize
    if (hi - lo <= leafSize){
            for (int i = lo; i < hi; i++) {
        node->rows.push_back(idx[i]);    // copy the spot i into the leaf, vector so push_back
        }
        numLeaves++;
        return node;
    }

    int d = depth % dim;
    int mid = lo + (hi-lo) / 2;
    std::nth_element(idx.begin() + lo, idx.begin() + mid, idx.begin() + hi,
        [&](int a, int b) { return data[a * dim + d] < data[b * dim + d]; }); // using the C++ standard library, not outside libraries
        // this is a partial sort, trivial to the overall function and implementation of KD tree
        // O(n) complexity
    

    node->splitDim = d; // split in this dimension 
    node->splitVal = data[idx[mid] * dim + d]; // split at this value

    node->left  = buildRec(lo, mid, depth + 1); // key recursive pattern: split at mid. lo, mid and then mid, hi
    node->right = buildRec(mid, hi, depth + 1); // increase depth, another key
    return node;
}

void KDTree::destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);        // basically like a post order destroy, which has always been the standard (AVL Tree implementation)
    destroy(node->right);       // now the right
    delete node;                // then the node itself, finally (down then up)
}

KDTree::~KDTree() { destroy(root); } // we use our recursive helper, just like the helpers from AVL Tree

void KDTree::printRec(const Node* node, int depth) const { // just for now, a printing checker
    if (node == nullptr) return;
    std::cout << std::string(2 * depth, ' ');

    if (node->isLeaf()) {
        std::cout << "leaf {";
        for (size_t i = 0; i < node->rows.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << node->rows[i];
        }
        std::cout << "}\n";
    } else {
        std::cout << "split d=" << node->splitDim << " at " << node->splitVal << "\n";
        printRec(node->left, depth + 1);
        printRec(node->right, depth + 1);
    }
}

void KDTree::print() const { printRec(root, 0); }

float KDTree::distSq(const float* q, int row) const {
    float sum = 0;
    for (int j = 0; j < dim; j++){ // multidim distance formula
        float diff = q[j] - data[row * dim + j];
        sum+= diff * diff;
    }
    return sum;
}

void KDTree::searchRec(const Node* node, const float* q, int k, Heap& heap, int& checked) const {
    if (node->isLeaf()){
        for (int row: node->rows){
            float dist = distSq(q, row);
            checked++;

            if ((int)heap.size() < k){
                heap.push({dist, row});
            } else if (dist < heap.top().first) { // check the distance on the current max in max heap (longest distance, first to go)
                heap.pop();
                heap.push({dist, row});
            }

        }
        return;
    }
    int d = node->splitDim;
    float s = node->splitVal;
    float gap = q[d] - s; // in one coordinate
    const Node* near;
    const Node* far;
    if (gap <= 0){
        near = node->left;
        far = node->right;
    } else {
        far = node->left;
        near = node->right;
    }

    searchRec(near, q, k, heap, checked); 
    if ((int)heap.size() < k || gap * gap < heap.top().first) {
        searchRec(far, q, k, heap, checked);
    }
}

std::vector<std::pair<int, float>> KDTree::query(const float* q, int k) { // return the vector of returns from queries, and scores! not dist
    if (k <= 0) return {};
    Heap heap;

    int checked = 0;
    if (root != nullptr) { // i mean it cant be empty right, all trees do this, but still have a checker
        if (maxLeaves < 0) searchRec(root, q, k, heap, checked);   // exact mode
        else               searchCapped(q, k, heap, checked);      // capped mode
    } 
    lastCheckedCount = checked;

    std::vector<std::pair<int, float>> result; // prepare
    while (!heap.empty()) {
        float dist = heap.top().first;
        int row    = heap.top().second; // int not float, almost forgot
        heap.pop();
        result.push_back({row, 1.0f - dist / 2.0f});   // score = 1 − distsq/2, in documentation
    }
    std::reverse(result.begin(), result.end());        // heap gave worst flip to best first, another std library algorithm
    // not essential to functioning of the KD Tree
    return result;
}

void KDTree::searchCapped(const float* q, int k, Heap& heap, int& checked) const {
    using Branch = std::pair<float, const Node*>;    // doing this just saves me the annoyment 
    std::priority_queue<Branch, std::vector<Branch>, std::greater<Branch>> toVisit;   // we need a min heap of branches, signature seen in lectures

    toVisit.push({0.0f, root});
    int leavesVisited = 0;

    while (!toVisit.empty() && leavesVisited < maxLeaves) {
       float bound = toVisit.top().first; // the current top bound in the minHeap
       const Node* node = toVisit.top().second;
       toVisit.pop();

        if ((int)heap.size() == k && bound >= heap.top().first) { // == because function handles no bigger than k
            continue; // check against the heap of results, if it cannot improve it, do not bother
        }

        while (!node->isLeaf()) {
            float gap = q[node->splitDim] - node->splitVal; // the query array at the specific dimension - the splitting value of the node
            const Node* near;
            const Node* far;
            if (gap <= 0){
                near = node->left; // simple logic to see which side is the nearest 
                far = node->right;
            } else {
                near = node->right;
                far = node->left;
            }

            toVisit.push({gap * gap, far}); // push the farther one
            node = near; // this helps me traverse to an actual leaf
        }

        for (int row: node->rows){
            float dist = distSq(q, row);
            checked++;
            if ((int)heap.size() < k){
                heap.push({dist, row});
            } else if (dist < heap.top().first){
                heap.pop();
                heap.push({dist, row});
            }
        }
        leavesVisited++;
    }
}