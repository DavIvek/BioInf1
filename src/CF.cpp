#include <cstdlib>
#include <functional>
#include <string>

#include  "CF.hpp"

// Constructor
CuckooFilter::CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level):
    number_of_buckets(number_of_buckets), fingerprint_size(fingerprint_size), bucket_size(bucket_size), current_size(0), max_kicks(500), current_level(current_level) {
        accept_values = true;
    }

// Destructor
CuckooFilter::~CuckooFilter() {
    if (child0 != nullptr) {
        delete child0;
    }
    if (child1 != nullptr) {
        delete child1;
    }
}

// Insert an item into the filter
std::optional<std::size_t> CuckooFilter::insert(const std::string& item) {
    
    if (current_size >= capacity()) {
        return std::nullopt;
    }

    buckets_new[5].bit_array[5] = 'a';

    std::size_t index1 = hash(item) % number_of_buckets;
    std::size_t fingerprint = hash(item) % (1 << fingerprint_size);

    std::size_t index2 = index1 ^ hash(fingerprint);

    // save f - current_level bits from the fingerprint
    auto saved_bits = fingerprint & ((1 << current_level) - 1);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    std::size_t index_to_use = (rand() % 2) ? index1 : index2;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index_to_use][i].insert(fingerprint)) {
            current_size++;
            return std::nullopt;
        }
    }

    for (std::size_t i = 0; i < max_kicks; i++) {
        std::size_t bucket_index = rand() % bucket_size;
        std::size_t temp_fingerprint = buckets[index_to_use][bucket_index].fingerprint;
        buckets[index_to_use][bucket_index].fingerprint = fingerprint;
        fingerprint = temp_fingerprint;

        index_to_use = index_to_use ^ hash(fingerprint);

        for (std::size_t j = 0; j < bucket_size; j++) {
            if (buckets[index_to_use][j].insert(fingerprint)) {
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

bool CuckooFilter::isFull() const {
    return current_size >= capacity();
}

// Capacity of the filter
std::size_t CuckooFilter::capacity() const {
    return number_of_buckets * bucket_size * 0.9375;
}

std::size_t CuckooFilter::hash(const std::string& item) const {
    std::hash<std::string> hash_fn;
    return hash_fn(item);
}

std::size_t CuckooFilter::hash(const std::size_t item) const {
    std::hash<std::size_t> hash_fn;
    return hash_fn(item);
}