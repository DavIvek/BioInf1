#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <random>
#include "LDCF.hpp"
#include "dynamicbloomfilter.h"

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
    const std::size_t num_strings = 100000;
    const std::size_t string_length = 10;
    const double false_positive_rate = 0.01;
    const std::size_t set_size = 100000;
    const std::size_t expected_levels = 3;

    // Generate random strings
    auto strings = generate_random_strings(num_strings, string_length);

    // Initialize LogarithmicDynamicCuckooFilter
    LogarithmicDynamicCuckooFilter ldcf(false_positive_rate, set_size, expected_levels);

    //Instantiate Bloom Filter
    DynamicBloomFilter bloom_filter(set_size, false_positive_rate, expected_levels);

    // Insert strings into both filters and measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& str : strings) {
        ldcf.insert(str);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ldcf_insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (const auto& str : strings) {
        // take pointer to char
        auto c_str = str.c_str();
        bloom_filter.insertItem(c_str);
    }
    end = std::chrono::high_resolution_clock::now();
    auto bloom_insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();

    // now check if the inserted strings are present
    int elements_not_present_ldcf = 0;
    for (const auto& str : strings) {
        if (!ldcf.contains(str)) {
            elements_not_present_ldcf++;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    auto ldcf_check_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    int elements_not_present_bloom = 0;
    for (const auto& str : strings) {
        // take pointer to char
        auto c_str = str.c_str();
        if (!bloom_filter.queryItem(c_str)) {
            elements_not_present_bloom++;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    auto bloom_check_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "LDCF Insert Time: " << ldcf_insert_time << " ms\n";
    std::cout << "Bloom Filter Insert Time: " << bloom_insert_time << " ms\n";
    std::cout << "LDCF Check Time: " << ldcf_check_time << " ms\n";
    std::cout << "Bloom Filter Check Time: " << bloom_check_time << " ms\n";
    std::cout << "Elements not present in LDCF: " << elements_not_present_ldcf << "\n";
    std::cout << "Elements not present in Bloom Filter: " << elements_not_present_bloom << "\n";

    return 0;
}