/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2017 Julien Eychenne <jeychenne@gmail.com>                                                            *
 *                                                                                                                     *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not   *
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.                                      *
 *                                                                                                                     *
 * Date: 2017.07.08                                                                                                    *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <codecvt>
#include <locale>
#include <sstream>

#include "unicode.hpp"
#include "extra/utf8proc/utf8proc.h"
#include "extra/utf8.hpp"


#define MAX_READ 4

namespace runtime { namespace unicode {


static inline
bool fast_check(char32_t codepoint) { return codepoint < 0x110000; }


char32_t to_upper(char32_t codepoint)
{
	return (char32_t)utf8proc_toupper((utf8proc_int32_t)codepoint);
}

char32_t to_lower(char32_t codepoint)
{
	return (char32_t)utf8proc_tolower((utf8proc_int32_t)codepoint);
}

char32_t to_tile(char32_t codepoint)
{
	return (char32_t)utf8proc_totitle((utf8proc_int32_t)codepoint);
}

bool is_valid(char32_t codepoint)
{
	return utf8proc_codepoint_valid((utf8proc_int32_t)codepoint);
}

bool is_space(char32_t codepoint)
{
	if (! fast_check(codepoint)) return false;
	return utf8proc_get_property((utf8proc_int32_t)codepoint)->category == UTF8PROC_CATEGORY_ZS;
}

bool is_upper(char32_t codepoint)
{
	if (! fast_check(codepoint)) return false;
	return utf8proc_get_property((utf8proc_int32_t)codepoint)->category == UTF8PROC_CATEGORY_LU;
}

bool is_lower(char32_t codepoint)
{
	if (! fast_check(codepoint)) return false;
	return utf8proc_get_property((utf8proc_int32_t)codepoint)->category == UTF8PROC_CATEGORY_LL;
}

bool is_title(char32_t codepoint)
{
	if (! fast_check(codepoint)) return false;
	return utf8proc_get_property((utf8proc_int32_t)codepoint)->category == UTF8PROC_CATEGORY_LT;
}

bool is_math(char32_t codepoint)
{
	if (! fast_check(codepoint)) return 0;
	return utf8proc_get_property((utf8proc_int32_t)codepoint)->category == UTF8PROC_CATEGORY_SM;
}

std::u16string utf8_to_utf16(const char *s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conversion;
	return conversion.from_bytes(s);
}

std::string utf16_to_utf8(const std::u16string &s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conversion;
	return conversion.to_bytes(s);
}

template <class internT, class externT, class stateT>
struct codecvt : std::codecvt<internT,externT,stateT>
{ ~codecvt(){} };


std::u32string utf8_to_utf32(const char *s)
{
	std::wstring_convert<codecvt<char32_t,char,std::mbstate_t>,char32_t> conversion;
	return conversion.from_bytes(s);
}

std::string utf32_to_utf8(const std::u32string &s)
{
	std::wstring_convert<codecvt<char32_t,char,std::mbstate_t>,char32_t> conversion;
	return conversion.to_bytes(s);
}

size_t strlen(const char *str, size_t len)
{
	size_t result = std::string::npos;
	bool ok = utf8_strlen(str, len, &result);

	if (!ok) {
		throw error("Invalid UTF-8 String");
	}
	
	return size_t(result);
}

bool is_letter(char32_t codepoint)
{
	if (!fast_check(codepoint)) return false;
	utf8proc_propval_t cat = utf8proc_get_property((utf8proc_int32_t)codepoint)->category;

	return cat >= UTF8PROC_CATEGORY_LU && cat <= UTF8PROC_CATEGORY_LO;
}

size_t write_codepoint(char32_t cp, char *dest)
{
	return (size_t) utf8proc_encode_char((utf8proc_int32_t)cp, (utf8proc_uint8_t*)dest);
}

char32_t next(const char *str, size_t index, size_t *nread)
{
	char32_t codepoint = 0;

	utf8proc_ssize_t count = utf8proc_iterate((uint8_t*)(str + index), MAX_READ, (int32_t*) &codepoint);

	if (count < 0)
	{
		if (nread) {
			*nread = string::npos;
		}

		throw error("Problem reading code point at index %", index);
	}
	else if (nread)
	{
		*nread = (size_t) count;
	}

	return codepoint;
}

/* utf8proc doesn't have a function to iterate backwards, so we roll our own. */
char32_t previous(const char *text, size_t index, size_t *nread)
{
	int32_t codepoint = 0;

	auto *str = (text) + index - 1;
	size_t len = 1;

	// Move back until we have a legal code point or we reach the start of the string.
	while (!is_legal_utf8(str, len))
	{
		if (str <= text)
		{
			throw error("Problem reading code point at index %", index);
		}
		str--; len++;
	}

	utf8proc_ssize_t ret = utf8proc_iterate((const uint8_t*)(text + index - len), len, &codepoint);

	if (ret < 0)
	{
		throw error("Problem reading code point at index %", index);
	}

	if (nread) *nread = (size_t) ret;

	return (char32_t) codepoint;
}

size_t offset(const char *str, size_t len, size_t index)
{
	for (size_t i = 0; i < len; )
	{
		if (i == index) {
			return i;
		}
		size_t nread = 0;
		next(str, i, &nread);
		i += nread;
	}

	throw error("Invalid code point index %", index);
}

std::string encode(char32_t cp)
{
	std::string result(5, '\0');
	auto dest = const_cast<char*>(result.data());
	auto count = (size_t) utf8proc_encode_char((utf8proc_int32_t)cp, reinterpret_cast<utf8proc_uint8_t*>(dest));
	result.resize(count);

	return result;
}


bool is_legal_utf8(const char *source, size_t len)
{
	return utf8_is_valid(source, len);
}

}} // namespace runtime::unicode
