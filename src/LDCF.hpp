#ifndef LOG_DCF_HPP
#define LOG_DCF_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <optional>

#include "CF.hpp"

/**
 * A logarithmic dynamic cuckoo filter implementation.
 */
class LogarithmicDynamicCuckooFilter {
public:
    /**
     * Constructor.
     * 
     * @param false_positive_rate The desired false positive rate.
     * @param set_size The expected number of items in the set.
     * @param expected_levels The expected number of levels in the filter.
     */
    LogarithmicDynamicCuckooFilter(const double false_positive_rate, const std::size_t set_size, const std::size_t expected_levels);

    /**
     * Destructor.
     */
    ~LogarithmicDynamicCuckooFilter();

    /**
     * Insert an item into the filter.
     * 
     * @param item The item to insert.
     */
    void insert(const std::string& item);

    /**
     * Check if an item is in the filter.
     * 
     * @param item The item to check.
     * @return True if the item is in the filter, false otherwise.
     */
    bool contains(const std::string& item) const;

    /**
     * Remove an item from the filter.
     * 
     * @param item The item to remove.
     * @return True if the item was removed, false otherwise.
     */
    bool remove(const std::string& item);

    /**
     * Get the filter's size.
     * 
     * @return The number of items in the filter.
     */
    std::size_t size() const { return size_; }

    /**
     * Get the filter's capacity.
     * 
     * @return The maximum number of items the filter can hold.
     */
    std::size_t capacity() const;

private:
    int false_positive_rate;
    std::size_t set_size;
    std::size_t size_;

    std::size_t number_of_buckets;
    std::size_t fingerprint_size;

    CuckooFilter* root;
    
    /**
     * Hash a string.
     * 
     * @param item The string to hash.
     * @return The hash of the string.
     */
    std::size_t hash(const std::string& item) const;

    /**
     * Get the prefix of the fingerprint.
     * 
     * @param fingerprint The fingerprint.
     * @param current_level The current level.
     * @param fingerprintSize The size of the fingerprint.
     * @return The prefix of the fingerprint.
     */
    bool getPrefix(const std::size_t fingerprint, const int current_level, const std::size_t fingerprintSize) const;
};

#endif // LOG_DCF_HPP