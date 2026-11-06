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

    // all of this is prog 2 basics, reading from bin file.
    
    int header[2];        // read first 2                      
    file.read(reinterpret_cast<char*>(header), sizeof(header)); // read into header
    if (!file) {
        std::cerr << "Could not read header: " << filename << std::endl;
        return false;
    }
    n = header[0]; // number of objects
    dim = header[1]; // dimension of objects

    vectors.resize((size_t)n * dim);  // keep the size to its needed size
    // reinterpret_cast<char*>(vectors.data())
    file.read(reinterpret_cast<char*>(vectors.data()), vectors.size() * sizeof(float)); // read for every objects 
    if (!file) {
        std::cerr << "File shorter than its n * dim " << filename << std::endl;
        return false;
    }
    return true;
}

static std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current += '"';        
                i++;
            } else {
                inQuotes = !inQuotes;   
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current); 
            current.clear();
        } else {
            current += c;             
        }
    }
    fields.push_back(current);         
    return fields;
}


bool loadMovies(const std::string& filename, std::vector<Movie>& movies) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);    

    while (std::getline(file, line)) {               
        if (!line.empty() && line.back() == '\r') line.pop_back();  
        std::vector<std::string> f = splitCSVLine(line);
        if (f.size() < 5) {
            std::cerr << "Bad line: " << line << std::endl;
            return false;
        }

        Movie m;
        m.id          = std::stoi(f[1]);
        m.title       = f[2];
        m.year        = f[3].empty() ? 0 : std::stoi(f[3]);
        m.poster_path = f[4];
        movies.push_back(m);
    }
    return true;
}
