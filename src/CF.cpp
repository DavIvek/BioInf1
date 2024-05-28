#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <optional>
#include <string>
#include <cstring>
#include <iostream>

#include  "CF.hpp"

// Constructor
CuckooFilter::CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level):
    bucket_size(bucket_size), current_size(0), max_kicks(100), current_level(current_level) {
        accept_values = true;
        child0 = nullptr;
        child1 = nullptr;

        this->number_of_buckets = nextPowerOfTwo(number_of_buckets);

        buckets.reserve(this->number_of_buckets);

        this->fingerprint_size = fingerprint_size - current_level;
        if (this->fingerprint_size < 1) {
            this->fingerprint_size = 1;
        }

        full_slots.reserve(this->number_of_buckets * bucket_size);

        // Initialize the empty slots to false
        for (std::size_t i = 0; i < this->number_of_buckets * bucket_size; i++) {
            full_slots.push_back(false);
        }

        auto bits_per_bucket = bucket_size * this->fingerprint_size;
        auto bytes_per_bucket = (bits_per_bucket + 7) >> 3;
        if (bytes_per_bucket == 0) {
            bytes_per_bucket = 1;
        }
        for (std::size_t i = 0; i < this->number_of_buckets; i++) {
            buckets[i].bit_array = new char[bytes_per_bucket]; // Allocate memory for the bucket
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
std::optional<std::pair<uint32_t, uint32_t>> CuckooFilter::insert(const std::string& item) {
    
    if (current_size >= capacity()) {
        return std::nullopt;
    }

    uint32_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << fingerprint_size) - 1);

    uint32_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    // save f - current_level bits from the fingerprint
    uint32_t saved_bits = fingerprint & ((1 << current_level) - 1);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    uint32_t index_to_use = index1;

    // check how many of given fingerprint we already have in the buckets
    std::size_t counter = 0;
    for (std::size_t i = 0; i < bucket_size; i++) {
        if (full_slots[index1 * bucket_size + i] != false) {
            auto result1 = buckets[index1].read(i, fingerprint_size);
            if (result1 == fingerprint) {
                counter++;
            }
        }
        if (full_slots[index2 * bucket_size + i] != false) {
            auto result2 = buckets[index2].read(i, fingerprint_size);
            if (result2 == fingerprint) {
                counter++;
            }
        }
    }

    if (counter >= bucket_size) {
        return std::nullopt;
    }

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (full_slots[index_to_use * bucket_size + i] == false) {
            buckets[index_to_use].write(i, fingerprint, fingerprint_size);
            full_slots[index_to_use * bucket_size + i] = true;
            current_size++;
            return std::nullopt;
        }
    }
    uint32_t index_of_victim = index_to_use;
    for (std::size_t i = 0; i < max_kicks; i++) {
        std::size_t bucket_index = rand() % bucket_size;
        std::uint32_t temp_fingerprint = buckets[index_to_use].read(bucket_index, fingerprint_size);
        if (i != 0) {
            fingerprint >>= current_level;
        }

        buckets[index_to_use].write(bucket_index, fingerprint, fingerprint_size);

        fingerprint = temp_fingerprint;

        fingerprint <<= current_level;
        fingerprint |= saved_bits;

        index_of_victim = index_to_use;

        index_to_use = (index_to_use ^ hash(fingerprint)) % number_of_buckets;

        for (std::size_t j = 0; j < bucket_size; j++) {
            if (full_slots[index_to_use * bucket_size + j] == false) {
                fingerprint >>= current_level;
                buckets[index_to_use].write(j, fingerprint, fingerprint_size);
                full_slots[index_to_use * bucket_size + j] = true;
                current_size++;
                return std::nullopt;
            }
        }
    }

    accept_values = false;

    return std::make_optional(std::pair<uint32_t, std::uint32_t>(fingerprint, index_of_victim));
}

// Insert victim
void CuckooFilter::insert(std::optional<std::pair<uint32_t, uint32_t>> victim) {
    // now we take f - current_level bits from the fingerprint
    uint32_t fingerprint = victim->first;

    uint32_t index1 = victim->second;
    uint32_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    fingerprint >>= current_level;

    uint32_t index_to_use = index1;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (full_slots[index_to_use * bucket_size + i] == false) {
            buckets[index_to_use].write(i, fingerprint, fingerprint_size);
            full_slots[index_to_use * bucket_size + i] = true;
            current_size++;
            return;
        }
    }

    // we should not be here
    throw std::runtime_error("Victim could not be inserted");
}

bool CuckooFilter::contains(const std::string& item) const {
    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << fingerprint_size) - 1);

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    for (std::size_t i = 0; i < bucket_size; i++) {
        // check if those buckets are empty
        if (full_slots[index1 * bucket_size + i] != false) {
            auto result1 = buckets[index1].read(i, fingerprint_size);
            if (result1 == fingerprint) {
                return true;
            }
        }
        if (full_slots[index2 * bucket_size + i] != false) {
            auto result2 = buckets[index2].read(i, fingerprint_size);
            if (result2 == fingerprint) {
                return true;
            }
        }
    }

    return false;
}


bool CuckooFilter::remove(const std::string& item) {
    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << fingerprint_size) - 1);

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index1].read(i, fingerprint_size) == fingerprint && full_slots[index1 * bucket_size + i] == true) {
            full_slots[index1 * bucket_size + i] = false; // no need to delete the fingerprint
            current_size--;
            return true;
        }
        if (buckets[index2].read(i, fingerprint_size) == fingerprint && full_slots[index2 * bucket_size + i] == true) {
            full_slots[index2 * bucket_size + i] = false; // no need to delete the fingerprint
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
    return number_of_buckets * bucket_size * 0.9;
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

std::size_t CuckooFilter::nextPowerOfTwo(std::size_t n) const {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}