#include "loader.h"
#include <fstream>
#include <sstream>
#include <iostream>


bool loadVectors(const std::string& filename, std::vector<float>& vectors, int& n, int& dim ) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    //implementation

    return true;
}


bool loadMovies(const std::string& filename, std::vector<Movie>& movies) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    // implementation
    
    return true;
}