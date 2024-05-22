#ifndef LOG_DCF_HPP
#define LOG_DCF_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

#include "CF.hpp"

class LogarithmicDynamicCuckooFilter {
public:
    // Constructor
    LogarithmicDynamicCuckooFilter(const int false_positive_rate, const std::size_t set_size);

    // Destructor
    ~LogarithmicDynamicCuckooFilter();

    // Insert an item into the filter
    void insert(const std::string& item);

    // Check if an item is in the filter
    bool contains(const std::string& item) const;

    // Remove an item from the filter
    bool remove(const std::string& item);

    // Get the current number of items in the filter
    std::size_t size() const;

    // Get the filter's capacity
    std::size_t capacity() const;

private:
    int false_positive_rate;
    std::size_t set_size;

    CuckooFilter* root;

    // Hash function for generating indices
    std::size_t hash(const std::string& item) const;

    // Get prefix of fingerprint
    bool getPrefix(const std::size_t fingerprint, const int current_level, const std::size_t fingerprintSize) const;
};

#endif // LOG_DCF_HPP