/*
 * hashfunctions.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: liaoliangyi
 */

#include "hashfunctions.h"
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

// Helper function to convert hash to hex string
std::string hashToHex(size_t hash) {
    std::stringstream ss;
    ss << std::hex << std::setw(sizeof(size_t) * 2) << std::setfill('0') << hash;
    return ss.str();
}

char* HashFunc::sha1(const char* key) {
    static char result[sizeof(size_t) * 2 + 1]; // To hold hex string of hash

    // Use std::hash for demonstration purposes
    std::hash<std::string> hasher;
    size_t hash_value = hasher(key);

    // Convert hash to hex string
    std::string hexStr = hashToHex(hash_value);
    strncpy(result, hexStr.c_str(), sizeof(result));
    result[sizeof(result) - 1] = '\0'; // Ensure null-termination

    return result;
}

char* HashFunc::md5(const char* key) {
    static char result[sizeof(size_t) * 2 + 1]; // To hold hex string of hash

    // Use std::hash for demonstration purposes
    std::hash<std::string> hasher;
    size_t hash_value = hasher(key);

    // Convert hash to hex string
    std::string hexStr = hashToHex(hash_value);
    strncpy(result, hexStr.c_str(), sizeof(result));
    result[sizeof(result) - 1] = '\0'; // Ensure null-termination

    return result;
}
