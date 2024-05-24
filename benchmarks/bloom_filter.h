#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
#include <functional>
#include <bitset>
#include <string>

class BloomFilter {
public:
    BloomFilter(size_t size, size_t hashCount);
    void add(const std::string& item);
    bool contains(const std::string& item) const;

private:
    size_t m_size;
    size_t m_hashCount;
    std::vector<bool> m_bits;
    std::hash<std::string> m_hash;
};

#endif



