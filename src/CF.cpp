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
