#pragma once

#include <vector>
#include <unordered_map>



// Locality-sensitive hashing (LSH) for approximate nearest neighbor search
// using cosine similarity
//
// This implementation uses random hyperplanes to hash vectors into buckets
// based on their cosine similarity. An inputted movie gets a 1 for an arrow
// if it points the same direction as the arrow, and a 0 if it points in the
// opposite. Similar movies get the similar codes. We will build multiple
// tables to reduce the probability of false negatives.


class LSH {
    LSH(int bits = 16, int tables = 10, unsigned int seed = 21);

    
};