#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <random>
#include "bloom_filter.hpp"
#include "LDCF.hpp"

// Function to generate random strings
std::vector<std::string> generate_random_strings(std::size_t n, std::size_t length) {
    std::vector<std::string> strings;
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(97, 122); // lowercase letters

    for (std::size_t i = 0; i < n; ++i) {
        std::string str(length, '\0');
        for (std::size_t j = 0; j < length; ++j) {
            str[j] = dist(rng);
        }
        strings.push_back(str);
    }
    return strings;
}

int main() {
    const std::size_t num_strings = 1000000;
    const std::size_t string_length = 10;
    const double false_positive_rate = 0.0001;
    const std::size_t set_size = 1000000;
    const std::size_t expected_levels = 3;

    // Generate random strings
    auto strings = generate_random_strings(num_strings, string_length);

    // Initialize LogarithmicDynamicCuckooFilter
    LogarithmicDynamicCuckooFilter ldcf(false_positive_rate, set_size, expected_levels);

    // Initialize Bloom filter
    bloom_parameters parameters;
    parameters.projected_element_count = set_size;
    parameters.false_positive_probability = false_positive_rate;

    parameters.compute_optimal_parameters();

    //Instantiate Bloom Filter
    bloom_filter bloom_filter(parameters);

    // Insert strings into both filters and measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& str : strings) {
        ldcf.insert(str);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ldcf_insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (const auto& str : strings) {
        bloom_filter.insert(str);
    }
    end = std::chrono::high_resolution_clock::now();
    auto bloom_insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Check for false positives
    std::unordered_map<std::string, bool> string_map;
    for (const auto& str : strings) {
        string_map[str] = true;
    }

    std::size_t ldcf_false_positives = 0;
    std::size_t bloom_false_positives = 0;
    auto false_strings = generate_random_strings(num_strings, string_length);

    start = std::chrono::high_resolution_clock::now();
    for (const auto& str : false_strings) {
        if (ldcf.contains(str) && string_map.find(str) == string_map.end()) {
            ldcf_false_positives++;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto ldcf_check_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (const auto& str : false_strings) {
        if (bloom_filter.contains(str) && string_map.find(str) == string_map.end()) {
            bloom_false_positives++;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto bloom_check_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double ldcf_fp_rate = static_cast<double>(ldcf_false_positives) / num_strings;
    double bloom_fp_rate = static_cast<double>(bloom_false_positives) / num_strings;

    std::cout << "LDCF Insert Time: " << ldcf_insert_time << " ms\n";
    std::cout << "Bloom Filter Insert Time: " << bloom_insert_time << " ms\n";
    std::cout << "LDCF Check Time: " << ldcf_check_time << " ms\n";
    std::cout << "Bloom Filter Check Time: " << bloom_check_time << " ms\n";
    std::cout << "LDCF False Positive Rate: " << ldcf_fp_rate << "\n";
    std::cout << "Bloom Filter False Positive Rate: " << bloom_fp_rate << "\n";

    return 0;
}
