#include <cstddef>
#include <cstdint>
#include <iostream>
#include <bitset>   
#include <cmath>
#include <sys/types.h>

#include "CF.hpp"
#include "LDCF.hpp"

// Constructor
LogarithmicDynamicCuckooFilter::LogarithmicDynamicCuckooFilter(double false_positive_rate, std::size_t set_size, std::size_t expected_levels):
    size_(0) {
    number_of_buckets = set_size / (BUCKET_SIZE * expected_levels);
    auto single_CF_capacity = LOAD_FACTOR * number_of_buckets * BUCKET_SIZE;
    double b_2 = 2 * 4;

    auto single_false_positive_rate = 1 - pow(1 - false_positive_rate, single_CF_capacity / set_size);
    this->fingerprint_size = log2(b_2/single_false_positive_rate);
    this->fingerprint_size = ceil(this->fingerprint_size + expected_levels);
    if (this->fingerprint_size > BYTE_SIZE * 4) {
        this->fingerprint_size = BYTE_SIZE * 4; // max fingerprint size
    }

    root = new CuckooFilter(number_of_buckets, this->fingerprint_size, 0);
}

// Destructor
LogarithmicDynamicCuckooFilter::~LogarithmicDynamicCuckooFilter() {
    delete root;
}

// Insert an item into the filter
void LogarithmicDynamicCuckooFilter::insert(const std::string &item) {
    int current_level = 0;
    auto *current_CF = root;
    uint32_t fingerprint = CuckooFilter::hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);

    while (current_CF->isFull()) {
        if (getPrefix(fingerprint, current_level, current_CF->getFingerprintSize())) {
            if (current_CF->child0 == nullptr) {
                current_CF->child0 = new CuckooFilter(number_of_buckets, fingerprint_size, current_level + 1);
            }
            current_CF = current_CF->child0;
        } else {
            if (current_CF->child1 == nullptr) {
                current_CF->child1 = new CuckooFilter(number_of_buckets, fingerprint_size, current_level + 1);
            }
            current_CF = current_CF->child1;
        }
        current_level++;
    }

    auto victim = current_CF->insert(item, fingerprint);
    if (victim.has_value()) {
        current_CF->child0 = new CuckooFilter(number_of_buckets, fingerprint_size, current_level + 1);
        current_CF->child1 = new CuckooFilter(number_of_buckets, fingerprint_size, current_level + 1);
        if (getPrefix(victim->fingerprint, current_level, current_CF->getFingerprintSize())) {
            current_CF->child0->insert(victim.value());
        } else {
            current_CF->child1->insert(victim.value());
        }
    }
    size_++;
}

// Check if an item is in the filter
bool LogarithmicDynamicCuckooFilter::contains(const std::string &item) const {
    CuckooFilter *current_CF = root;
    int current_level = 0;
    uint32_t fingerprint = CuckooFilter::hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);
    while (true) {
        if (current_CF->contains(item, fingerprint)) {
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
bool LogarithmicDynamicCuckooFilter::remove(const std::string &item) {
    CuckooFilter *current_CF = root;
    int current_level = 0;
    uint32_t fingerprint = CuckooFilter::hash(item);
    fingerprint = fingerprint & ((1 << current_CF->getFingerprintSize()) - 1);
    while (true) {
        if (current_CF->contains(item, fingerprint)) {
            size_--;
            current_CF->acceptValues(true);
            return current_CF->remove(item, fingerprint);
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

bool LogarithmicDynamicCuckooFilter::getPrefix(std::size_t fingerprint, int current_level, std::size_t fingerprintSize) {
    // put the one to the position of the current level
    uint32_t mask = 1 << current_level;
    return (fingerprint & mask) == 0;
}