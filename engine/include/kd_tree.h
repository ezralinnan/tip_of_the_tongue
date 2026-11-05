#pragma once
#include <vector>
#include <queue>

struct Node {
    // internal nodes use these
    int splitDim = -1;        // which coordinate from 0 to 383
    float splitVal = 0.0f;    // compare q[splitDim] against this (where each left right split happens)
    Node* left = nullptr; // children of course
    Node* right = nullptr;

    // leaves use this
    std::vector<int> rows;    // up to leafSize row numbers, which in this case will be up to 16

    bool isLeaf() const { return left == nullptr; }
};

class KDTree{
    private:

        const float* data = nullptr; // pointer to the array
        int nMovies = 0;
        int dim= 0; // coordinates, 384 from ML embedder
        int leafSize = 16;
        Node* root = nullptr; // need a pointer to the start of the tree, usual practice
        std::vector<int> idx; // the build rearranges the movies into the leaves, which means rearranging row numbers (dimension splits)
        // each leaf is one block of idx
        void printRec(const Node* node, int depth) const;
        Node* buildRec(int lo, int hi, int depth);   // the recursive build
        void destroy(Node* node);
        float distSq(const float* q, int row) const;
        using Heap = std::priority_queue<std::pair<float, int>>; // spares me the headache of the massive signature
        void searchRec(const Node* node, const float* q, int k, Heap& heap) const;

    public:
        void print() const;
        explicit KDTree(int leafSize = 16) : leafSize(leafSize) {}
        ~KDTree();                                   // deletes the whole tree

        KDTree(const KDTree&) = delete;              // no copying because of pointers to nodes, safety first
        KDTree& operator=(const KDTree&) = delete;  // no assignment operator either

        void build(const float* data, int n, int dim);
        std::vector<std::pair<int, float>> query(const float* q, int k); // int k is how many results to return, useful for top 5
};