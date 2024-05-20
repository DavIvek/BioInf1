#ifndef CUCKOO_FILTER_HPP
#define CUCKOO_FILTER_HPP

#include <cstdint>
#include <vector>

class CuckooFilter {
public:
    // Constructor
    CuckooFilter(const std::size_t table_size, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level);

    // Destructor
    ~CuckooFilter();

    // Insert an item into the filter
    bool insert(const std::string& item);

    // Check if an item is in the filter
    bool contains(const std::string& item) const;

    // Remove an item from the filter
    bool remove(const std::string& item);

    // Get the current number of items in the filter
    std::size_t size() const;

    // Get the filter's capacity
    std::size_t capacity() const;

    // Get the fingerprint size
    std::size_t fingerprintSize() const;

private:
    bool is_full;
    std::size_t table_size;
    std::size_t fingerprint_size;
    std::size_t bucket_size;
    std::size_t max_kicks;
    std::size_t current_size;

    CuckooFilter* child0;
    CuckooFilter* child1;
    CuckooFilter* parent;

    // Hash function for generating indices
    std::size_t hash(const std::string& item) const;
};

#endif // CUCKOO_FILTER_HPP