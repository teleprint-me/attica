/**
 * @file include/utf8/string.h
 * @brief ASCII and UTF-8 Structured API.
 * High-level API for abstracting Codepoint and Raw pointer-to-char interfaces.
 *
 * - A UTF-8 byte represents a valid ASCII or UTF-8 code point.
 * - Library functions are prefixed with `utf8_`.
 * - Low Level: Byte functions are prefixed with `utf8_byte_`.
 * - Mid Level: Raw pointer-to-char functions are prefixed with `utf8_raw_`.
 * - High Level: Interface functions are prefixed with `utf8_string_`.
 */

#ifndef UTF8_STRING_H
#define UTF8_STRING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct UTF8String {
    char* buffer; // literal string in memory
    size_t capacity; // size in bytes
    size_t length; // number of code points
} UTF8String;

typedef struct UTF8StringSplit {
    UTF8String* parts;
    size_t capacity;
} UTF8StringSplit;

// --- UTF-8 String Operations ---

UTF8String* utf8_string_create(const char* buffer);
size_t utf8_string_capacity(const UTF8String* string); // Capacity of the buffer in bytes
size_t utf8_string_length(const UTF8String* string); // Number of codepoints in buffer
UTF8String* utf8_string_copy(const UTF8String* string); // Copy the entire object
UTF8String* utf8_string_copy_n(const UTF8String* string, const size_t capacity); // Copy up to n bytes
UTF8String* utf8_string_concat(const UTF8String* head, const UTF8String* tail); // Join strings
void utf8_string_free(UTF8String* string);

// --- UTF-8 Split Operations ---

UTF8StringSplit* utf8_string_split(const char* buffer, const char* delimiter);
UTF8StringSplit* utf8_string_split_regex(const char* buffer, const char* pattern);
int utf8_string_split_append(UTF8StringSplit* split, const UTF8String* string);
UTF8String* utf8_string_split_pop(UTF8StringSplit* split);
UTF8String* utf8_string_split_join(UTF8StringSplit* split, const char* delimiter);
void utf8_string_split_free(UTF8StringSplit* split); 

#endif // UTF8_STRING_H
