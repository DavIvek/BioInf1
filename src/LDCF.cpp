#include "CF.hpp"
#include "LDCF.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <bitset>   
#include <math.h>
#include <sys/types.h>

// Constructor
LogarithmicDynamicCuckooFilter::LogarithmicDynamicCuckooFilter(const std::size_t false_positive_rate, const std::size_t set_size, const std::size_t expected_levels, const std::size_t fingerprint_size):
    size_(0), fingerprint_size(fingerprint_size) {
    // optimal bucket size -> 4
    number_of_buckets = set_size / (4 * expected_levels);
    auto load_factor = 0.935;
    auto single_CF_capacity = load_factor * number_of_buckets * 4;

    auto single_false_positive_rate = 1 - pow(1 - false_positive_rate, single_CF_capacity / set_size);

    root = new CuckooFilter(number_of_buckets, fingerprint_size, 4, 0);
}

// Destructor
LogarithmicDynamicCuckooFilter::~LogarithmicDynamicCuckooFilter() {
    delete root;
}

// Insert an item into the filter
void LogarithmicDynamicCuckooFilter::insert(const std::string& item) {
    int current_level = 0;
    auto current_CF = root;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);

    while (current_CF->isFull()) {
        if (getPrefix(fingerprint, current_level, current_CF->getFingerprintSize())) {
            if (current_CF->child0 == nullptr) {
                current_CF->child0 = new CuckooFilter(number_of_buckets, fingerprint_size, 4, current_level + 1);
            }
            current_CF = current_CF->child0;
        } else {
            if (current_CF->child1 == nullptr) {
                current_CF->child1 = new CuckooFilter(number_of_buckets, fingerprint_size, 4, current_level + 1);
            }
            current_CF = current_CF->child1;
        }
        current_level++;
    }

    auto victim = current_CF->insert(item);
    if (victim.has_value()) {
        current_CF->child0 = new CuckooFilter(number_of_buckets, fingerprint_size, 4, current_level);
        current_CF->child1 = new CuckooFilter(number_of_buckets, fingerprint_size, 4, current_level);
        if (getPrefix(victim->first, current_level, current_CF->getFingerprintSize())) {
            current_CF->child0->insert(victim.value());
        } else {
            current_CF->child1->insert(victim.value());
        }
    }
    size_++;
}

// Check if an item is in the filter
bool LogarithmicDynamicCuckooFilter::contains(const std::string& item) const {
    CuckooFilter *current_CF = root;
    int current_level = 0;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);
    while (true) {
        if (current_CF->contains(item)) {
            return true;
        }
        if (getPrefix(fingerprint, current_level, current_CF->getFingerprintSize())) {
            if (current_CF->child0 == nullptr) {
                return false;
            }
            current_CF = current_CF->child0;
        } else {
            if (current_CF->child1 == nullptr) {
                return false;
            }
            current_CF = current_CF->child1;
        }
        current_level++;
    }
}

// Remove an item from the filter
bool LogarithmicDynamicCuckooFilter::remove(const std::string& item) {
    CuckooFilter *current_CF = root;
    int current_level = 0;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);
    while (true) {
        if (current_CF->contains(item)) {
            size_--;
            current_CF->acceptValues(true);
            return current_CF->remove(item);
        }
        if (getPrefix(fingerprint, current_CF->current_level, current_CF->getFingerprintSize())) {
            if (current_CF->child0 == nullptr) {
                return false;
            }
            current_CF = current_CF->child0;
        } else {
            if (current_CF->child1 == nullptr) {
                return false;
            }
            current_CF = current_CF->child1;
        }
        current_level++;
    }
}

std::size_t LogarithmicDynamicCuckooFilter::hash(const std::string& item) const {
    std::hash<std::string> hash_fn;
    return hash_fn(item);
}

bool LogarithmicDynamicCuckooFilter::getPrefix(const std::size_t fingerprint, const int current_level, const std::size_t fingerprintSize) const {
    // put the one to the position of the current level
    uint32_t mask = 1 << current_level;
    return (fingerprint & mask) == 0;
}