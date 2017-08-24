/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2017 Julien Eychenne <jeychenne@gmail.com>                                                            *
 *                                                                                                                     *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not   *
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.                                      *
 *                                                                                                                     *
 * Date: 2017.07.08                                                                                                    *
 *                                                                                                                     *
 * Purpose: low-level Unicode manipulation routines.                                                                   *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_UNICODE_HPP
#define ELVEA_UNICODE_HPP

#include <string>

#include "string.hpp"

namespace runtime { namespace unicode {

typedef uint8_t char8_t;

// UTF end-of-line character
const char32_t eol = 0x0000000A;

// UTF end-of-file character
const char32_t eof = 0xFFFF;

// UTF end-of-text character
const char32_t eot = 0x00000003;


char32_t to_upper(char32_t codepoint);
char32_t to_lower(char32_t codepoint);
char32_t to_tile(char32_t codepoint);

bool is_valid(char32_t codepoint);

bool is_space(char32_t codepoint);
bool is_upper(char32_t codepoint);
bool is_lower(char32_t codepoint);
bool is_title(char32_t codepoint);
bool is_math(char32_t codepoint);

static inline
bool is_digit(char32_t codepoint) { return isdigit((int) codepoint); }

static inline
bool is_ascii(char32_t codepoint) { return codepoint < 128; }

std::u16string utf8_to_utf16(const char *s);
std::string utf16_to_utf8(const std::u16string &s);

std::u32string utf8_to_utf32(const char *s);
std::string utf32_to_utf8(const std::u32string &s);

size_t strlen(const char *str, size_t len);

bool is_letter(char32_t codepoint);

size_t write_codepoint(char32_t cp, char *dest);

char32_t next(const char *str, size_t index, size_t *nread = nullptr);
char32_t previous(const char *text, size_t index, size_t *nread = nullptr);

size_t offset(const char *str, size_t len, size_t index);

std::string encode(char32_t cp);

bool is_legal_utf8(const char *source, size_t len);

}} // namespace runtime::unicode

#endif // ELVEA_UNICODE_HPP
