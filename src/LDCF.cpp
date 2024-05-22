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

    while (current_CF->isFull()) {
        current_level++;
        if (getPrefix(current_CF->getFingerprintSize(), current_level, current_CF->getFingerprintSize())) {
            current_CF = current_CF->child0;
        } else {
            current_CF = current_CF->child1;
        }
    }

    auto victim = current_CF->insert(item);
    if (victim.has_value()) {
        current_CF->child0 = new CuckooFilter(set_size, 4, 4, current_level + 1);
        current_CF->child1 = new CuckooFilter(set_size, 4, 4, current_level + 1);
        if (getPrefix(current_CF->getFingerprintSize(), current_level, current_CF->getFingerprintSize())) {
            current_CF->child0->insert(victim.value());
        } else {
            current_CF->child1->insert(victim.value());
        }
    }
}

// Check if an item is in the filter
bool LogarithmicDynamicCuckooFilter::contains(const std::string& item) const {
    CuckooFilter *current_CF = root;
    while (true) {
        if (current_CF->contains(item)) {
            return true;
        }
        if (getPrefix(current_CF->getFingerprintSize(), current_CF->current_level, current_CF->getFingerprintSize())) {
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
    }
}

// Remove an item from the filter
bool LogarithmicDynamicCuckooFilter::remove(const std::string& item) {
    CuckooFilter *current_CF = root;
    while (true) {
        if (current_CF->contains(item)) {
            return current_CF->remove(item);
        }
        if (getPrefix(current_CF->getFingerprintSize(), current_CF->current_level, current_CF->getFingerprintSize())) {
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