#include "CF.hpp"
#include "LDCF.hpp"

// Constructor
LogarithmicDynamicCuckooFilter::LogarithmicDynamicCuckooFilter(const int false_positive_rate, const std::size_t set_size):
    false_positive_rate(false_positive_rate), set_size(set_size) {
    root = new CuckooFilter(set_size, 4, 4, 0);
}

// Destructor
LogarithmicDynamicCuckooFilter::~LogarithmicDynamicCuckooFilter() {
    delete root;
}