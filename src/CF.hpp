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
            //std::cout << "byte: " << std::bitset<8>(*(uint8_t*)(bit_array_copy)) << std::endl;
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
            *(uint16_t*)(bit_array_copy) = fingerprint;
        }
        else if (fingerprint_size <= 24) {
            bit_array_copy += (position + (position << 1));
            *(uint32_t*)(bit_array_copy) &= 0xFF000000; // Clear the upper 24 bits
            *(uint32_t*)(bit_array_copy) |= fingerprint; // Set the upper 24 bits
        }
        else {
            // four bytes -> write to whole bytes
            bit_array_copy += (position << 2);
            *(uint32_t*)(bit_array_copy) = fingerprint;
        }
    }

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

class CuckooFilter {
public:

    int current_level;

    CuckooFilter* child0;
    CuckooFilter* child1;

    // Constructor
    CuckooFilter(const std::size_t number_of_buckets, const std::size_t fingerprint_size, const std::size_t bucket_size, int current_level);

    // Destructor
    ~CuckooFilter();

    // Insert an item into the filter
    std::optional<std::pair<uint32_t, uint32_t>> insert(const std::string& item);

    void insert(std::optional<std::pair<uint32_t, uint32_t>> victim);

    // Check if an item is in the filter
    bool contains(const std::string& item) const;

    // Remove an item from the filter
    bool remove(const std::string& item);

    // Get the current number of items in the filter
    std::size_t size() const;

    // Get the filter's capacity
    std::size_t capacity() const;

    // Get the fingerprint size
    std::size_t getFingerprintSize() const { return fingerprint_size; }

    bool isFull() const;

private:
    std::size_t number_of_buckets;
    std::size_t fingerprint_size;
    std::size_t bucket_size;
    std::size_t max_kicks;
    std::size_t current_size;

    bool accept_values;

    std::unordered_set<uint32_t> kicked_fingerprints;

    std::vector<Bucket> buckets;

    // Hash function for generating indices
    std::size_t hash(const std::string& item) const;

    std::size_t hash(const std::size_t item) const;

    uint32_t createFingerprint(const std::string& item) const;
};

#endif // CUCKOO_FILTER_HPP