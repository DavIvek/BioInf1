#include <functional>
#include <string>

#include  "CF.hpp"

// Constructor
CuckooFilter::CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level):
    number_of_buckets(number_of_buckets), fingerprint_size(fingerprint_size), bucket_size(bucket_size), current_size(0), is_full(false), max_kicks(500), current_level(current_level) {}

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
bool CuckooFilter::insert(const std::string& item) {
    if (current_size >= capacity()) {
        return false;
    }

    std::size_t index = hash(item) % number_of_buckets;
    std::size_t fingerprint = hash(item) % (1 << fingerprint_size);

    // now we take f - current_level bits from the fingerprint
    fingerprint >>= current_level;

    for (std::size_t i = 0; i < bucket_size; i++) {
        if (buckets[index][i].insert(fingerprint)) {
            current_size++;
            return true;
        }
    }

    std::size_t i = 0;
    while (i < max_kicks) {
        std::size_t random_bucket = rand() % bucket_size;
        std::size_t temp_fingerprint = buckets[index][random_bucket].fingerprint;
        buckets[index][random_bucket].fingerprint = fingerprint;
        fingerprint = temp_fingerprint;
        index = (index ^ hash(std::to_string(fingerprint))) % number_of_buckets;

        for (std::size_t j = 0; j < bucket_size; j++) {
            if (buckets[index][j].insert(fingerprint)) {
                current_size++;
                return true;
            }
        }

        i++;
    }
    
    return false;
}

// Capacity of the filter
std::size_t CuckooFilter::capacity() const {
    return number_of_buckets * bucket_size * 0.9375;
}

std::size_t CuckooFilter::hash(const std::string& item) const {
    std::hash<std::string> hash_fn;
    return hash_fn(item);
}