#pragma once
#include <string>
#include <vector>

// movie struct to hold movie information from movies.csv
struct Movie {
    int id = 0;
    std::string title;
    int year = 0; // year of release, 0 if unknown
    std::string poster_path;
};



// reads vectors.bin
// returns true if successful, false if error
// vectors come back as one flat array of floats, with n vectors of dimension dim

bool loadVectors(const std::string& filename, std::vector<float>& vectors, int& n, int& dim);

// reads movies.csv
// returns true if successful, false if error
// movies come back as a vector of Movie structs

bool loadMovies(const std::string& filename, std::vector<Movie>& movies);