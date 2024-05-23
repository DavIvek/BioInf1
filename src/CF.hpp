#ifndef CUCKOO_FILTER_HPP
#define CUCKOO_FILTER_HPP

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

struct Bucket {
    char *bit_array;

    void write(std::size_t position, uint32_t fingerprint, std::size_t fingerprint_size) {        
        if (fingerprint_size <= 4) {
            // even or odd position
            bit_array += (position >> 1);
            if (position & 1) {
                // odd position -> write to the left
                *(uint8_t*)(bit_array) &= 0xF0;
                *(uint8_t*)(bit_array) |= (fingerprint);
            }
            else {
                // even position -> write to the right
                *(uint8_t*)(bit_array) &= 0x0F;
                *(uint8_t*)(bit_array) |= (fingerprint << 4);
            }
        }
        else if (fingerprint_size <= 8) {
            // one byte -> write to whole byte
            *(uint8_t*)(bit_array) = fingerprint;
        }
        else if (fingerprint_size <= 12) {
            bit_array += (position + (position >> 1));
            if (position & 1) {
                *(uint16_t*)(bit_array) &= 0x000F; // Clear the upper 12 bits
                *(uint16_t*)(bit_array) |= (fingerprint << 4); // Set the upper 12 bits
            }
            else {
                *(uint16_t*)(bit_array) &= 0xF000; // Clear the lower 12 bits
                *(uint16_t*)(bit_array) |= (fingerprint); // Set the lower 12 bits
            }
        }
        else if (fingerprint_size <= 16) {
            // two bytes -> write to whole bytes
            *(uint16_t*)(bit_array) = fingerprint;
        }
        else if (fingerprint_size <= 24) {
            bit_array += (position + (position << 1));
            *(uint32_t*)(bit_array) &= 0xFF000000; // Clear the upper 24 bits
            *(uint32_t*)(bit_array) |= fingerprint; // Set the upper 24 bits
        }
        else {
            // four bytes -> write to whole bytes
            *(uint32_t*)(bit_array) = fingerprint;
        }
    }

    uint32_t read(std::size_t position, std::size_t fingerprint_size) {
        uint32_t fingerprint = 0;

        if (fingerprint_size <= 4) {
            bit_array += (position >> 1);
            if (position & 1) {
                // odd position -> read from the left
                fingerprint = (*(uint8_t*)(bit_array) & 0xF);
            }
            else {
                // even position -> read from the right
                fingerprint = (*(uint8_t*)(bit_array) >> 4 & 0xF);
            }
        }
        else if (fingerprint_size <= 8) {
            fingerprint = *(uint8_t*)(bit_array) & 0xFF;
        }
        else if (fingerprint_size <= 12) {
            bit_array += (position + (position >> 1));
            if (position & 1) {
                fingerprint = (*(uint16_t*)(bit_array) >> 4 & 0xFFF); // Read the upper 12 bits
            }
            else {
                fingerprint = (*(uint16_t*)(bit_array) & 0xFFF); // Read the lower 12 bits
            }
        }
        else if (fingerprint_size <= 16) {
            fingerprint = *(uint16_t*)(bit_array) & 0xFFFF;
        }
        else if (fingerprint_size <= 24) {
            bit_array += (position + (position << 1));
            fingerprint = (*(uint32_t*)(bit_array) >> 4); // Read the upper 24 bits
        }
        else {
            fingerprint = *(uint32_t*)(bit_array) & 0xFFFFFFFF;
        }

        return fingerprint & ((1 << fingerprint_size) - 1); // right part of the bit expression is to clear the upper bits 
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
    std::optional<uint32_t> insert(const std::string& item);

    void insert(const std::size_t victim);

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

    Bucket *buckets;

    // Hash function for generating indices
    std::size_t hash(const std::string& item) const;

    std::size_t hash(const std::size_t item) const;
};

#endif // CUCKOO_FILTER_HPP