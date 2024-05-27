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

    // write fingerprint to the bucket
    void write(std::size_t position, uint32_t fingerprint, std::size_t fingerprint_size) {   
        char *bit_array_copy= bit_array;
        if (fingerprint_size <= 4) {
            // even or odd position
            bit_array_copy += (position >> 1);
            if (position & 1) {
                *((uint8_t*)(bit_array_copy)) &= 0xF0;
                *((uint8_t*)(bit_array_copy)) |= (fingerprint);
            }
            else {
                *((uint8_t*)(bit_array_copy)) &= 0x0F;
                *((uint8_t*)(bit_array_copy)) |= (fingerprint << 4);
            }
        }
        else if (fingerprint_size <= 8) {
            // one byte -> write to whole byte
            bit_array_copy += position;
            *(uint8_t*)(bit_array_copy) = fingerprint;
        }
        else if (fingerprint_size <= 12) {
            bit_array_copy += (position + (position >> 1));
            if (position & 1) {
                *(uint16_t*)(bit_array_copy) &= 0x000F; // Clear the upper 12 bits
                *(uint16_t*)(bit_array_copy) |= (fingerprint << 4); // Set the upper 12 bits
            }
            else {
                *(uint16_t*)(bit_array_copy) &= 0xF000; // Clear the lower 12 bits
                *(uint16_t*)(bit_array_copy) |= (fingerprint); // Set the lower 12 bits
            }
        }
        else if (fingerprint_size <= 16) {
            // two bytes -> write to whole bytes
            bit_array_copy += (position << 1);
            *(uint16_t*)(bit_array_copy) = fingerprint;
        }
        else if (fingerprint_size <= 24) {
            bit_array_copy += (position + (position << 1));
            *(uint32_t*)(bit_array_copy) &= 0xFF000000;  // Clear the upper 24 bits
            *(uint32_t*)(bit_array_copy) |= fingerprint; // Set the upper 24 bits
        }
        else {
            // four bytes -> write to whole bytes
            bit_array_copy += (position << 2);
            *(uint32_t*)(bit_array_copy) = fingerprint;
        }
    }

    // read fingerprint from the bucket
    uint32_t read(std::size_t position, std::size_t fingerprint_size) const {
        uint32_t fingerprint = 0;
        const char *bit_array_copy = bit_array;
        if (fingerprint_size <= 4) {
            bit_array_copy += (position >> 1);
            uint8_t bits_8 = *(uint8_t*)(bit_array_copy);
            if (position & 1) {
                // odd position -> read from the left
                fingerprint = bits_8 & 0xF;
            }
            else {
                // even position -> read from the right
                fingerprint = (bits_8 >> 4) & 0xF;
            }
        }
        else if (fingerprint_size <= 8) {
            bit_array_copy += position;
            fingerprint = *(uint8_t*)(bit_array_copy) & 0xFF;
        }
        else if (fingerprint_size <= 12) {
            bit_array_copy += (position + (position >> 1));
            if (position & 1) {
                fingerprint = (*(uint16_t*)(bit_array_copy) >> 4 & 0xFFF); // Read the upper 12 bits
            }
            else {
                fingerprint = (*(uint16_t*)(bit_array_copy) & 0xFFF); // Read the lower 12 bits
            }
        }
        else if (fingerprint_size <= 16) {
            bit_array_copy += (position << 1);
            fingerprint = *(uint16_t*)(bit_array_copy) & 0xFFFF;
        }
        else if (fingerprint_size <= 24) {
            bit_array_copy += (position + (position << 1));
            uint32_t bits_32 = *(uint32_t*)(bit_array_copy);
            fingerprint = bits_32 & 0xFFFFFF; 
        }
        else {
            bit_array_copy += (position << 2);
            fingerprint = *(uint32_t*)(bit_array_copy) & 0xFFFFFFFF;
            return fingerprint;
        }
        uint32_t mask = (1 << fingerprint_size) - 1;
        return fingerprint & mask;                          // right part of the bit expression is to clear the upper bits 
                                                            // if the left shift operation resulted in 00010000, subtracting 1 gives 00001111
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

    // Hash function for generating indices
    std::size_t hash(const std::string& item) const;

    std::size_t hash(const std::size_t item) const;

    uint32_t createFingerprint(const std::string& item) const;

    std::size_t nextPowerOfTwo(std::size_t n) const;
};

#endif // CUCKOO_FILTER_HPP