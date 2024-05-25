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
    number_of_buckets(number_of_buckets), bucket_size(bucket_size), current_size(0), max_kicks(100), current_level(current_level) {
        accept_values = true;
        child0 = nullptr;
        child1 = nullptr;

        buckets.reserve(number_of_buckets);

        this->fingerprint_size = fingerprint_size;

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
std::optional<std::pair<uint32_t, uint32_t>> CuckooFilter::insert(const std::string& item) {
    
    if (current_size >= capacity()) {
        return std::nullopt;
    }

    uint32_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << fingerprint_size) - 1);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    if (fingerprint == 0) {
        fingerprint = 1;
    }

    uint32_t fingerprint_size_temp = this->fingerprint_size - current_level;

    uint32_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    // save f - current_level bits from the fingerprint
    uint32_t saved_bits = fingerprint & ((1 << current_level) - 1);

    uint32_t index_to_use = index1; // should be random but cant due to mod

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index_to_use].read(i, fingerprint_size_temp) == 0) {
            buckets[index_to_use].write(i, fingerprint, fingerprint_size_temp);
            current_size++;
            return std::nullopt;
        }
    }
    uint32_t index_of_victim = index_to_use;
    for (std::size_t i = 0; i < max_kicks; i++) {
        std::size_t bucket_index = rand() % bucket_size;
        std::uint32_t temp_fingerprint = buckets[index_to_use].read(bucket_index, fingerprint_size_temp);
        if (kicked_fingerprints.find(temp_fingerprint) == kicked_fingerprints.end()) {
            kicked_fingerprints.insert(temp_fingerprint);
        }
        else {
            // std::cout << "size of kicked_fingerprints: " << kicked_fingerprints.size() << std::endl;
            continue;
        }
        buckets[index_to_use].write(bucket_index, fingerprint, fingerprint_size_temp);
        fingerprint = temp_fingerprint;

        index_of_victim = index_to_use;

        index_to_use = (index_to_use ^ hash(fingerprint)) % number_of_buckets;

        for (std::size_t j = 0; j < bucket_size; j++) {
            if (buckets[index_to_use].read(j, fingerprint_size_temp) == 0) {
                buckets[index_to_use].write(j, fingerprint, fingerprint_size_temp);
                current_size++;
                return std::nullopt;
            }
        }
    }

    // now if we are here we have to restore the fingerprint
    fingerprint <<= current_level;
    fingerprint |= saved_bits;

    accept_values = false;

    return std::make_optional(std::pair<uint32_t, std::uint32_t>(fingerprint, index_of_victim));
}

// Insert victim
void CuckooFilter::insert(std::optional<std::pair<uint32_t, uint32_t>> victim) {
    // now we take f - current_level bits from the fingerprint
    uint32_t fingerprint = victim->first;

    fingerprint >>= current_level;
    if (fingerprint == 0) {
        fingerprint = 1;
    }

    uint32_t index1 = victim->second;
    uint32_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    uint32_t index_to_use = index1;

    uint32_t fingerprint_size_temp = this->fingerprint_size - current_level;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index_to_use].read(i, fingerprint_size_temp) == 0) {
            buckets[index_to_use].write(i, fingerprint, fingerprint_size_temp);
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

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    uint32_t fingerprint_size_temp = this->fingerprint_size - current_level;

    if (fingerprint == 0) {
        fingerprint = 1;
    }

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    for (std::size_t i = 0; i < bucket_size; i++) {
        auto result1 = buckets[index1].read(i, fingerprint_size_temp);
        auto result2 = buckets[index2].read(i, fingerprint_size_temp);
        if (result1 == fingerprint || result2 == fingerprint) {
            return true;
        }
    }

    return false;
}


bool CuckooFilter::remove(const std::string& item) {
    std::size_t index1 = hash(item) % number_of_buckets;
    uint32_t fingerprint = hash(item);
    fingerprint = fingerprint & ((1 << fingerprint_size) - 1);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    uint32_t fingerprint_size_temp = this->fingerprint_size - current_level;

    if (fingerprint == 0) {
        fingerprint = 1;
    }

    std::size_t index2 = (index1 ^ hash(fingerprint)) % number_of_buckets;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index1].read(i, fingerprint_size_temp) == fingerprint) {
            buckets[index1].write(i, 0, fingerprint_size_temp);
            current_size--;
            return true;
        }
        if (buckets[index2].read(i, fingerprint_size_temp) == fingerprint) {
            buckets[index2].write(i, 0, fingerprint_size_temp);
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