#ifndef CUCKOO_FILTER_HPP
#define CUCKOO_FILTER_HPP

#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <bitset>

struct Bucket {
    char *bit_array;

    // Write fingerprint to the bucket
    void write(std::size_t position, uint32_t fingerprint, std::size_t fingerprint_size) {
        std::size_t bit_offset = position * fingerprint_size;
        std::size_t byte_offset = bit_offset / 8;
        bit_offset %= 8;

        uint64_t mask = (1ULL << fingerprint_size) - 1;
        fingerprint &= mask;

        uint64_t *target = reinterpret_cast<uint64_t*>(bit_array + byte_offset);
        *target &= ~(mask << bit_offset);
        *target |= (static_cast<uint64_t>(fingerprint) << bit_offset);
    }

    // Read fingerprint from the bucket
    uint32_t read(std::size_t position, std::size_t fingerprint_size) const {
        std::size_t bit_offset = position * fingerprint_size;
        std::size_t byte_offset = bit_offset / 8;
        bit_offset %= 8;

        uint64_t mask = (1ULL << fingerprint_size) - 1;

        const uint64_t *target = reinterpret_cast<const uint64_t*>(bit_array + byte_offset);
        uint64_t fingerprint = (*target >> bit_offset) & mask;

        return static_cast<uint32_t>(fingerprint);
    }
};

/**
 * Cuckoo Filter implementation
 */
class CuckooFilter {
public:

    int current_level;

    CuckooFilter* child0;
    CuckooFilter* child1;

    /**
     * Constructor
     * @param number_of_buckets Number of buckets in the filter
     * @param fingerprint_size Size of the fingerprint in bits
     * @param bucket_size Number of entries in each bucket
     */
    CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level);

    /**
     * Destructor
     */
    ~CuckooFilter();

    /**
     * Insert an item into the filter
     * @param item Item to insert
     * @return std::nullopt if the filter is full, otherwise the victim item and its index
     */
    std::optional<std::pair<uint32_t, uint32_t>> insert(const std::string& item);

    /**
     * Check if an item is in the filter
     * @param victim victim to insert
     */
    void insert(std::optional<std::pair<uint32_t, uint32_t>> victim);

    /**
     * Check if an item is in the filter
     * @param item Item to check
     * @return True if the item is in the filter, false otherwise
     */
    bool contains(const std::string& item) const;

    /**
     * Remove an item from the filter
     * @param item Item to remove
     * @return True if the item was removed, false otherwise
     */
    bool remove(const std::string& item);

    /**
     * Get the filter's size
     * @return The number of items in the filter
     */
    std::size_t size() const;

    /**
     * Get the filter's capacity
     * @return The maximum number of items the filter can hold
     */
    std::size_t capacity() const;

    /**
     * Get the filter's fingerprint size
     * @return The size of the fingerprint in bits
     */
    std::size_t getFingerprintSize() const { return fingerprint_size; }

    /**
     * Check if the filter is full
     * @return True if the filter is full, false otherwise
     */
    bool isFull() const;

    /**
     * Accept values
     * @param accept True if the filter should accept values, false otherwise
     */
    void acceptValues(bool accept) { accept_values = accept; }

private:
    std::size_t number_of_buckets;
    std::size_t fingerprint_size;
    std::size_t bucket_size;
    std::size_t max_kicks;
    std::size_t current_size;

    bool accept_values;

    std::vector<Bucket> buckets;

    std::vector<bool> full_slots; 

    /**
     * Hash a string
     * @param item The string to hash
     * @return The hash of the string
     */
    std::size_t hash(const std::string& item) const;

    /**
     * Hash a size_t
     * @param item The size_t to hash
     * @return The hash of the size_t
     */
    std::size_t hash(const std::size_t item) const;

    /**
     * Get next power of two
     * @param n The number to get the next power of two for
     * @return The next power of two
     */
    std::size_t nextPowerOfTwo(std::size_t n) const;
};

#endif // CUCKOO_FILTER_HPP