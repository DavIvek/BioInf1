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

// Insert an item into the filter
void LogarithmicDynamicCuckooFilter::insert(const std::string& item) {
    int current_level = 0;
    auto current_CF = root;

    while (!current_CF->acceptValues()) {
        current_level++;
        if (getPrefix(current_CF->getFingerprintSize(), current_level, current_CF->getFingerprintSize())) {
            current_CF = current_CF->child0;
        } else {
            current_CF = current_CF->child1;
        }
    }

    auto victim = current_CF->insert(item);
    if (victim.has_value()) {
        current_CF->accept_values = false;
    }
}

std::size_t LogarithmicDynamicCuckooFilter::hash(const std::string& item) const {
    std::hash<std::string> hash_fn;
    return hash_fn(item);
}

bool LogarithmicDynamicCuckooFilter::getPrefix(const std::size_t fingerprint, const int current_level, const std::size_t fingerprintSize) const {
    if (fingerprint >> (fingerprintSize - 1) == 0) return true;
    return false;
}