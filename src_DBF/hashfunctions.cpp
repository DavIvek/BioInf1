/*
 * hashfunctions.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: liaoliangyi
 */

#include "hashfunctions.h"
#include <cstring>
#include <openssl/evp.h>

char* HashFunc::sha1(const char* key) {
    EVP_MD_CTX *mdctx;
    unsigned char value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    // Allocate and initialize the context
    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        // Handle error
        return NULL;
    }

    // Initialize the digest context
    if (EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Update the digest with the key
    if (EVP_DigestUpdate(mdctx, key, strlen(key)) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Finalize the digest
    if (EVP_DigestFinal_ex(mdctx, value, &md_len) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Clean up
    EVP_MD_CTX_free(mdctx);

    return (char*)value;
}

char* HashFunc::md5(const char* key) {
    EVP_MD_CTX *mdctx;
    unsigned char value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    // Allocate and initialize the context
    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        // Handle error
        return NULL;
    }

    // Initialize the digest context
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Update the digest with the key
    if (EVP_DigestUpdate(mdctx, key, strlen(key)) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Finalize the digest
    if (EVP_DigestFinal_ex(mdctx, value, &md_len) != 1) {
        // Handle error
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Clean up
    EVP_MD_CTX_free(mdctx);

    return (char*)value;
}
