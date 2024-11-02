// src/main.cpp
#include <iostream>
#include <fstream>
#include <string>

#include "../include/json_parser/json.hpp"
#include "../include/json_parser/jsonParser.hpp"
#include "../include/json_parser/jsonEvaluator.hpp"

Json callParser(const std::string& filePath);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "\033[38;5;208m" << "Usage: " << argv[0] << " <file_path>" << "\033[0m" << std::endl;
        return 1;
    }

    try {
        Json json = callParser(argv[1]);

        // If there is an expression evaluate it, if not just print the json
        if (argc == 2) {
            std::cout << json << std::endl;
            return 0;
        } else if (argc == 3) {
            Json result = JsonEvaluator::evaluate(json, argv[2]);
            std::cout << result << std::endl;
            return 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" "Error: " << e.what() << "\033[0m" << std::endl;
        return 1;
    }

    return 0;
}

// Helper function to read a file and call JsonParser::parse
Json callParser(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    return JsonParser::parse(content);
}