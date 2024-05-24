#include <cstdint>
#include <cstdlib>
#include <functional>
#include <string>
#include <cstring>
#include <iostream>

#include  "CF.hpp"

// Constructor
CuckooFilter::CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level):
    number_of_buckets(number_of_buckets), fingerprint_size(fingerprint_size), bucket_size(bucket_size), current_size(0), max_kicks(500), current_level(current_level) {
        accept_values = true;
        child0 = nullptr;
        child1 = nullptr;

        buckets.reserve(number_of_buckets);

        auto bits_per_bucket = bucket_size * fingerprint_size;
        auto bytes_per_bucket = (bits_per_bucket + 7) >> 3;
        for (std::size_t i = 0; i < number_of_buckets; i++) {
            buckets[i].bit_array = new char[bytes_per_bucket];
            // 0 out the bucket
            memset(buckets[i].bit_array, 0, bytes_per_bucket);
        }
    }

// Destructor
CuckooFilter::~CuckooFilter() {
    if (child0 != nullptr) {
        delete child0;
    }
    if (child1 != nullptr) {
        delete child1;
    }
    for (std::size_t i = 0; i < number_of_buckets; ++i) {
        delete[] buckets[i].bit_array;
    }
}

// Insert an item into the filter
std::optional<uint32_t> CuckooFilter::insert(const std::string& item) {
    
    if (current_size >= capacity()) {
        return std::nullopt;
    }

    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item) % (1 << fingerprint_size);
    if (fingerprint == 0) {
        fingerprint = 1;
    }

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    // save f - current_level bits from the fingerprint
    uint32_t saved_bits = fingerprint & ((1 << current_level) - 1);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    std::size_t index_to_use = (rand() % 2) ? index1 : index2;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index_to_use].read(i, fingerprint_size) == 0) {
            buckets[index_to_use].write(i, fingerprint, fingerprint_size);
            uint32_t temp = buckets[index_to_use].read(i, fingerprint_size);
            if (fingerprint != temp) {
                throw std::runtime_error("Error inserting fingerprint for fingerprint size: " + std::to_string(fingerprint_size));
            }
            current_size++;
            return std::nullopt;
        }
    }

    for (std::size_t i = 0; i < max_kicks; i++) {
        std::size_t bucket_index = rand() % bucket_size;
        std::size_t temp_fingerprint = buckets[index_to_use].read(bucket_index, fingerprint_size);
        buckets[index_to_use].write(bucket_index, fingerprint, fingerprint_size);
        fingerprint = temp_fingerprint;

        index_to_use = index_to_use ^ hash(fingerprint) % number_of_buckets;

        for (std::size_t j = 0; j < bucket_size; j++) {
            if (buckets[index_to_use].read(j, fingerprint_size) == 0) {
                buckets[index_to_use].write(j, fingerprint, fingerprint_size);
                current_size++;
                return std::nullopt;
            }
        }
    }

    // now if we are here we have to restore the fingerprint
    fingerprint <<= current_level;
    fingerprint |= saved_bits;

    accept_values = false;

    return fingerprint;  
}

bool CuckooFilter::contains(const std::string& item) const {
    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item) % (1 << fingerprint_size);
    if (fingerprint == 0) {
        fingerprint = 1;
    }

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    for (std::size_t i = 0; i < bucket_size; i++) {
        auto result1 = buckets[index1].read(i, fingerprint_size);
        auto result2 = buckets[index2].read(i, fingerprint_size);
        if (result1 == fingerprint || result2 == fingerprint) {
            return true;
        }
    }

    return false;
}


bool CuckooFilter::remove(const std::string& item) {
    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item) % (1 << fingerprint_size);
    if (fingerprint == 0) {
        fingerprint = 1;
    }

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index1].read(i, fingerprint_size) == fingerprint) {
            buckets[index1].write(i, 0, fingerprint_size);
            current_size--;
            return true;
        }
        if (buckets[index2].read(i, fingerprint_size) == fingerprint) {
            buckets[index2].write(i, 0, fingerprint_size);
            current_size--;
            return true;
        }
    }

    return false;
}

bool CuckooFilter::isFull() const {
    return current_size >= capacity() || !accept_values;
}

// Capacity of the filter
std::size_t CuckooFilter::capacity() const {
    return number_of_buckets * bucket_size * 0.8;
}

// Size of the filter
std::size_t CuckooFilter::size() const {
    return current_size;
}

std::size_t CuckooFilter::hash(const std::string& item) const {
    std::hash<std::string> hash_fn;
    return hash_fn(item);
}

std::size_t CuckooFilter::hash(const std::size_t item) const {
    std::hash<std::size_t> hash_fn;
    return hash_fn(item);
}