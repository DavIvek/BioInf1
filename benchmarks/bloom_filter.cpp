#include "bloom_filter.h"

BloomFilter::BloomFilter(size_t size, size_t hashCount)
    : m_size(size), m_hashCount(hashCount), m_bits(size) {}

void BloomFilter::add(const std::string& item) {
    for (size_t i = 0; i < m_hashCount; ++i) {
        size_t hashValue = m_hash(item + std::to_string(i)) % m_size;
        m_bits[hashValue] = true;
    }
}

bool BloomFilter::contains(const std::string& item) const {
    for (size_t i = 0; i < m_hashCount; ++i) {
        size_t hashValue = m_hash(item + std::to_string(i)) % m_size;
        if (!m_bits[hashValue]) {
            return false;
        }
    }
    return true;
}
