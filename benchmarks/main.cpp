#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <random> 
#include "LDCF.hpp" 

std::vector<std::string> read_sequences_from_fq(const std::string& filename) {
    std::vector<std::string> sequences;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file " << filename << std::endl;
        return sequences;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '@') {
            std::getline(file, line); // Sequence line
            sequences.push_back(line);
            std::getline(file, line); // Skip '+'
            std::getline(file, line); // Skip quality line
        }
    }
    file.close();
    return sequences;
};


// function for generation random strings
std::vector<std::string> generate_random_strings(std::size_t num_strings, std::size_t string_length) {
    std::vector<std::string> strings;
    strings.reserve(num_strings);
    const char charset[] = "ACGT";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 3);

    for (std::size_t i = 0; i < num_strings; ++i) {
        std::string str(string_length, 0);
        for (std::size_t j = 0; j < string_length; ++j) {
            str[j] = charset[dist(rng)];
        }
        strings.push_back(str);
    }
    return strings;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <reads_1.fq> <reads_2.fq> <num_strings> <string_length> <false_positive_rate> <expected_levels>" << std::endl;
        return 1;
    }

    std::string file1 = argv[1];
    std::string file2 = argv[2];
    std::size_t num_strings = std::stoul(argv[3]);
    std::size_t string_length = std::stoul(argv[4]);
    double false_positive_rate = std::stod(argv[5]);
    std::size_t expected_levels = std::stoul(argv[6]);

    // read data from files
    auto sequences1 = read_sequences_from_fq(file1);
    auto sequences2 = read_sequences_from_fq(file2);
    
    // bind all seq
    std::vector<std::string> all_sequences = sequences1;
    all_sequences.insert(all_sequences.end(), sequences2.begin(), sequences2.end());

    // trim 
    if (all_sequences.size() > num_strings) {
        all_sequences.resize(num_strings);
    }

    // init LogarithmicDynamicCuckooFilter
    LogarithmicDynamicCuckooFilter ldcf(false_positive_rate, num_strings, expected_levels);

    // time clock
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& seq : all_sequences) {
        ldcf.insert(seq);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ldcf_insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // test false positives
    std::unordered_map<std::string, bool> string_map;
    string_map.reserve(all_sequences.size());
    for (const auto& seq : all_sequences) {
        string_map.emplace(seq, true);
    }

    std::size_t ldcf_false_positives = 0;

    // random strings for false positives
    auto false_strings = generate_random_strings(num_strings / 10, string_length);

    start = std::chrono::high_resolution_clock::now();
    for (const auto& seq : false_strings) {
        if (ldcf.contains(seq) && string_map.find(seq) == string_map.end()) {
            ldcf_false_positives++;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto ldcf_check_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double ldcf_fp_rate = static_cast<double>(ldcf_false_positives) / (num_strings / 10);

    std::cout << "LDCF Insert Time: " << ldcf_insert_time << " ms\n";
    std::cout << "LDCF Check Time: " << ldcf_check_time << " ms\n";
    std::cout << "LDCF False Positive Rate: " << ldcf_fp_rate << "\n";

    return 0;
}
