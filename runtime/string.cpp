/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2017 Julien Eychenne                                                                                  *
 *                                                                                                                     *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not   *
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.                                      *
 *                                                                                                                     *
 * Software distributed under this License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either    *
 * express or implied. See the License for the specific language governing rights and limitations under the License.   *
 *                                                                                                                     *
 * Created: 2017.07.03                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <cassert>
#include "string.hpp"
#include "runtime/unicode.hpp"
#include "extra/boyer-moore-horspool/horspool.hpp"

#ifdef ELVEA_WITH_REGEX
#include <elvea/core/regex.hpp>
#endif



namespace runtime {

static inline size_t next_capacity(size_t n)
{
    assert(n != 0);
    return (n < 32) ? (n << 1) : (n + (n >> 1));
}

string::iterator::iterator(const string &s, size_t pos) :
		str(&s), pos(pos)
{}

string::iterator::iterator(const string::iterator &other) :
		str(other.str), pos(other.pos)
{}

string::iterator string::iterator::operator++(int)
{
	string::iterator old = *this;
	++(*this);

	return old;
}

string::iterator string::iterator::operator--(int)
{
	string::iterator old = *this;
	--(*this);

	return old;
}

string::iterator &string::iterator::operator=(const string::iterator &other)
{
	if (this != &other)
	{
		this->str = other.str;
		this->pos = other.pos;
	}

	return *this;
}

bool string::iterator::operator==(const string::iterator &other) const
{
	return this->pos == other.pos && this->str == other.str;
}

bool string::iterator::operator!=(const string::iterator &other) const
{
	return this->pos != other.pos || this->str != other.str;
}

string::codepoint_ref string::iterator::operator*() const
{
	return codepoint_ref(const_cast<iterator*>(this));
}

string::iterator::operator char32_t() const
{
	return codepoint();
}

string::iterator &string::iterator::operator++()
{
	size_t nread = 0;
	unicode::next(str->data(), pos, &nread);
	pos += nread;

	return *this;
}

string::iterator &string::iterator::operator--()
{
	size_t nread = 0;
	unicode::previous(str->data(), pos, &nread);
	pos -= nread;

	return *this;
}

string::iterator &string::iterator::operator=(char32_t codepoint)
{
	assign(codepoint);
	return *this;
}

string::iterator string::iterator::operator+(int i) const
{
	auto it(*this);
	std::advance(it, i);

	return it;
}

//String::iterator String::iterator::operator-(int i) const
//{
//	auto it(*this);
//
//	while (i-- > 0) {
//		--it;
//	}
//
//	return it;
//}

char32_t string::iterator::codepoint() const
{
	return pos < str->raw_size() ? unicode::next(str->data(), pos) : 0;
}

void string::iterator::assign(char32_t codepoint)
{
	size_t nread = 0;

	if (pos < str->raw_size()) {
		unicode::next(str->data(), pos, &nread);
	}
	const_cast<string *>(str)->raw_replace(pos, nread, unicode::encode(codepoint));
}

string::codepoint_ref::operator char32_t() const
{
	return iterator(*str, pos).codepoint();
}

string::codepoint_ref& string::codepoint_ref::operator=(const string::iterator &it)
{
	iterator(*str, pos).assign(it.codepoint());
	return *this;
}

string::codepoint_ref& string::codepoint_ref::operator=(const string::codepoint_ref &other)
{
	return (*this = iterator(*other.str, other.pos));
}

string::codepoint_ref &string::codepoint_ref::operator=(char32_t cp)
{
	iterator(*str, pos).assign(cp);
	return *this;
}

bool string::codepoint_ref::operator<(const string::codepoint_ref &other) const
{
	return iterator(*str, pos).codepoint() < iterator(*other.str, other.pos);
}

bool string::codepoint_ref::operator==(const string::codepoint_ref &other) const
{
	return iterator(*str, pos).codepoint() == iterator(*other.str, other.pos);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string::string()
{
	impl = empty_string();
	impl->retain();
}

string::string(const string &other) noexcept
{
	impl = other.impl;
	assert(impl);
	impl->retain();
}

string::string(string &&other) noexcept
{
	impl = other.impl;
	other.invalidate();
}

string::string(std::string_view str) :
	string(str.data(), str.size())
{ }

string::string(const std::string &str) :
	string(str.data(), str.size())
{ }

string::~string()
{
	release();
}

string::string(const char *str) :
	string(str, std::char_traits<char>::length(str))
{ }

string::string(const char *str, size_t len)
{
	impl = new storage(str, len);
}

string::string(size_t requested, string::allocate)
{
	impl = new storage(requested);
}

string::string(char32_t codepoint) :
	string(unicode::encode(codepoint))
{ }

void string::swap(string &other) noexcept
{
	std::swap(this->impl, other.impl);
}

string &string::operator=(const string &other)
{
	if (this != &other)
	{
		assert(impl);
		impl->release();
		impl = other.impl;
		assert(impl);
		impl->retain();
	}

	return *this;
}

string &string::operator=(string &&other) noexcept
{
	swap(other);
	return *this;
}

size_t string::size() const
{
	if (impl->utf8_len == npos) {
		return unicode::strlen(data(), raw_size());
	}

	return impl->utf8_len;
}

string string::clone() const
{
	return string(to_string_view());
}


bool string::operator==(const string &other) const
{
	return equal(other.data(), other.raw_size());
}

bool string::operator==(const std::string &other) const
{
	return equal(other.data(), other.size());
}

bool string::operator==(std::string_view other) const
{
	return equal(other.data(), other.size());
}

bool string::operator==(const char *str) const
{
	return equal(str, std::char_traits<char>::length(str));
}

bool string::operator==(char32_t codepoint) const
{
	return *this == unicode::encode(codepoint);
}

bool string::operator!=(const string &other) const
{
	return not_equal(other.data(), other.raw_size());
}

bool string::operator!=(const std::string &other) const
{
	return not_equal(other.data(), other.size());
}

bool string::operator!=(std::string_view other) const
{
	return not_equal(other.data(), other.size());
}

bool string::operator!=(const char *str) const
{
	return not_equal(str, std::char_traits<char>::length(str));
}

bool string::operator!=(char32_t codepoint) const
{
	return *this != unicode::encode(codepoint);
}

bool string::operator<(const string &other) const
{
	return less_than(other.data());
}

bool string::operator<(const std::string &other) const
{
	return less_than(other.data());
}

bool string::operator<(std::string_view other) const
{
	return less_than(other.data());
}

bool string::operator<(const char *str) const
{
	return less_than(str);
}

bool string::operator<(char32_t codepoint) const
{
	return *this < unicode::encode(codepoint);
}

bool string::operator>(const string &other) const
{
	return greater_than(other.data());
}

bool string::operator>(const std::string &other) const
{
	return greater_than(other.data());
}

bool string::operator>(std::string_view other) const
{
	return greater_than(other.data());
}

bool string::operator>(const char *str) const
{
	return greater_than(str);
}

bool string::operator>(char32_t codepoint) const
{
	return *this > unicode::encode(codepoint);
}

bool string::operator<=(const string &other) const
{
	return less_equal(other.data());
}

bool string::operator<=(const std::string &other) const
{
	return less_equal(other.data());
}

bool string::operator<=(std::string_view other) const
{
	return less_equal(other.data());
}

bool string::operator<=(const char *str) const
{
	return less_equal(str);
}

bool string::operator<=(char32_t codepoint) const
{
	return *this <= unicode::encode(codepoint);
}

bool string::operator>=(const string &other) const
{
	return greater_equal(other.data());
}

bool string::operator>=(const std::string &other) const
{
	return greater_equal(other.data());
}

bool string::operator>=(std::string_view other) const
{
	return greater_equal(other.data());
}

bool string::operator>=(const char *str) const
{
	return greater_equal(str);
}

bool string::operator>=(char32_t codepoint) const
{
	return *this >= unicode::encode(codepoint);
}

int string::compare(const string &other) const
{
	return compare(other.data());
}

int string::compare(const std::string &other) const
{
	return compare(other.data());
}

int string::compare(std::string_view other) const
{
	return compare(other.data());
}

int string::compare(const char *str) const
{
	return strcmp(data(), str);
}

int string::compare(char32_t codepoint) const
{
	return compare(unicode::encode(codepoint));
}

bool string::equal(const char *str, size_t len) const
{
	if (raw_size() != len) {
		return false;
	}

	if (data() == str) {
		return true;
	}

	return std::char_traits<char>::compare(data(), str, len) == 0;
}

bool string::not_equal(const char *str, size_t len) const
{
	return not equal(str, len);
}

bool string::less_than(const char *str) const
{
	return compare(str) < 0;
}

bool string::less_equal(const char *str) const
{
	return compare(str) <= 0;
}

bool string::greater_than(const char *str) const
{
	return compare(str) > 0;
}

bool string::greater_equal(const char *str) const
{
	return compare(str) >= 0;
}

bool string::starts_with(std::string_view prefix) const
{
	return starts_with(to_string_view(), prefix);
}

bool string::starts_with(char32_t codepoint) const
{
	if (unicode::is_ascii(codepoint)) {
		return not empty() && data()[0] == (char)codepoint;
	}

	return starts_with(unicode::encode(codepoint));
}

bool string::starts_with(std::string_view self, std::string_view prefix)
{
	if (prefix.size() > self.size()) {
		return false;
	}

	return std::char_traits<char>::compare(self.data(), prefix.data(), prefix.size()) == 0;
}

bool string::ends_with(std::string_view self, std::string_view suffix)
{
	if (suffix.size() > self.size()) {
		return false;
	}
	const char *start = self.data() + self.size() - suffix.size();

	return std::char_traits<char>::compare(start, suffix.data(), suffix.size()) == 0;
}

bool string::ends_with(std::string_view suffix) const
{
	return ends_with(to_string_view(), suffix);
}

bool string::ends_with(char32_t codepoint) const
{
	if (unicode::is_ascii(codepoint))
	{
		auto self = to_string_view();
		return not self.empty() && self.data()[self.size()-1] == (char)codepoint;
	}

	return ends_with(unicode::encode(codepoint));
}

size_t string::capacity() const
{
	return str().capacity();
}

size_t string::raw_find(std::string_view substring, size_t from) const
{
	return raw_find(to_string_view(), substring, from);
}

size_t string::raw_find(std::string_view self, std::string_view substring, size_t from)
{
	auto size = size_t(self.size() - from);
	auto str = reinterpret_cast<const uint8_t*>(self.data() + from);
	auto pattern = reinterpret_cast<const uint8_t*>(substring.data());
	auto result = search_horspool(str, size, pattern, size_t(substring.size()));

	return result < size ? result + from : invalid_index();
}

size_t string::raw_find(const char *substring, size_t len, size_t from) const
{
	return raw_find(std::string_view(substring, len), from);

}

size_t string::raw_find(char32_t codepoint, size_t from) const
{
	return raw_find(unicode::encode(codepoint));
}

bool string::contains(std::string_view substring) const
{
	return raw_find(substring) != invalid_index();
}

bool string::contains(char32_t codepoint) const
{
	return contains(unicode::encode(codepoint));
}

bool string::contains(std::string_view self, std::string_view substring)
{
	return raw_find(self, substring) != invalid_index();
}

size_t string::find(std::string_view substring, size_t from, bool raw_result) const
{
	return find(substring.data(), substring.size(), from, raw_result);
}

size_t string::find(const char *substring, size_t len, size_t from, bool raw_result) const
{
	from = raw_index(from);
	auto result = raw_find(substring, len, from);

	return raw_result ? result : index(result);
}

size_t string::find(char32_t codepoint, size_t from, bool raw_result) const
{
	return find(unicode::encode(codepoint), from, raw_result);
}

size_t string::raw_find_last(std::string_view substring, size_t to) const
{
	return raw_find_last(substring.data(), substring.size(), to);
}

size_t string::raw_find_last(const char *substring, size_t len, size_t to) const
{
	if (len == 0) {
		return invalid_index();
	}

	size_t current_size = raw_size();

	to = (to >= invalid_index()) ? current_size : raw_index(to);

	size_t i = to - len + 1;

	while (i-- > 0)
	{
		if (std::char_traits<char>::compare(data()+i, substring, len) == 0)
		{
			return i;
		}
	}

	return invalid_index();
}

size_t string::raw_find_last(char32_t codepoint, size_t to) const
{
	return raw_find_last(unicode::encode(codepoint), to);
}

size_t string::find_last(std::string_view substring, size_t to, bool raw_result) const
{
	return find_last(substring.data(), substring.size(), to, raw_result);
}

size_t string::find_last(const char *substring, size_t len, size_t to, bool raw_result) const
{
	to = (to >= invalid_index()) ? raw_size() : raw_index(to);
	auto result = raw_find_last(substring, len, to);

	return raw_result ? result : index(result);
}

size_t string::find_last(char32_t codepoint, size_t to, bool raw_result) const
{
	return find_last(unicode::encode(codepoint), to, raw_result);
}

size_t string::index(size_t pos) const
{
	if (pos < invalid_index())
	{
		auto it = cbegin();
		size_t count = 0;

		while (it.index() < pos) {
			++it; ++count;
		}

		if (it.index() != pos)
		{
			throw error("Byte index % is not a valid code point start position", pos);
		}

		pos = count;
	}

	return pos;
}

size_t string::raw_index(size_t pos) const
{
	if (pos != 0)
	{
		auto it = begin();
		std::advance(it, pos);
		pos = it.index();
	}

	return pos;
}

void string::unshare()
{
	if (this->shared())
	{
		string tmp = clone();
		this->swap(tmp);
	}
}

size_t string::invalid_index()
{
	return string::npos;
}

size_t string::max_size()
{
	return invalid_index() - 1;
}

string::iterator string::begin() noexcept
{
	return iterator(*this);
}

string::const_iterator string::begin() const noexcept
{
	return const_iterator(*this);
}

string::const_iterator string::cbegin() const noexcept
{
	return const_iterator(*this);
}

string::iterator string::end() noexcept
{
	return iterator(*this, raw_size());
}

string::const_iterator string::end() const noexcept
{
	return const_iterator(*this, raw_size());
}

string::const_iterator string::cend() const noexcept
{
	return const_iterator(*this, raw_size());
}

string string::raw_substr(size_t from, size_t to) const
{
	if (to == invalid_index()) {
		to = raw_size();
	}

	if (from >= raw_size() || to > raw_size() || from > to)
	{
		throw error("Invalid byte range [%:%] for substring", from, to);
	}

	return string(data() + from, to - from);
}

string string::substr(size_t from, size_t to) const
{
	from = raw_index(from);

	if (to == invalid_index()) {
		to = raw_size();
	}
	else {
		to = raw_index(to);
	}

	return raw_substr(from, to);
}

string string::left(size_t count) const
{
	auto it = begin();
	std::advance(it, count);

	return { data(), it.index() };
}

string string::right(size_t count) const
{
	size_t pos = raw_size();
	size_t nread = 0;

	for (size_t i = 0; i < count; ++i)
	{
		unicode::previous(data(), pos, &nread);

		// Check underflow
		if (pos < nread)
		{
			throw error("Cannot extract % code points from string of length %", count, size());
		}

		pos -= nread;
	}

	const char *start = data() + pos;
	size_t len = raw_size() - pos;

	return { start, len };
}

string string::reverse() const
{
	string result(capacity(), string::allocate());

	for (auto it = end(); it != begin();)
	{
		--it;
		result.append(*it);
	}

	return result;
}

string string::to_upper() const
{
	string result(capacity(), string::allocate());

	for (char32_t codepoint : *this) {
		result.append(unicode::to_upper(codepoint));
	}

	return result;
}

string string::to_lower() const
{
	string result(capacity(), string::allocate());

	for (char32_t codepoint : *this) {
		result.append(unicode::to_lower(codepoint));
	}

	return result;
}

bool string::is_upper() const
{
	for (auto c : *this)
	{
		if (not unicode::is_upper(c)) {
			return false;
		}
	}

	return true;
}

bool string::is_lower() const
{
	for (auto c : *this)
	{
		if (not unicode::is_lower(c)) {
			return false;
		}
	}

	return true;
}

void string::reserve(size_t requested)
{
	size_t new_capacity;
	bool needs_expansion = requested > capacity();

	if (not impl->shared() && not needs_expansion) {
		return;
	}

    new_capacity = next_capacity(std::max<size_t>(capacity(), 1));

	while (new_capacity < requested) {
        new_capacity = next_capacity(new_capacity);
	}

	string tmp(new_capacity, allocate());
	tmp.str() = this->str();
	swap(tmp);
}

string &string::append(std::string_view suffix)
{
	if (suffix.empty()) {
		return *this;
	}

	reserve(raw_size() + suffix.size() + 1);
	str().append(suffix.data(), suffix.size());
	reset_cache();

	return *this;
}

string &string::append(char32_t codepoint)
{
	if (unicode::is_ascii(codepoint))
	{
		reserve(raw_size() + 2);
		str().push_back((char)codepoint);
		reset_cache();

		return *this;
	}

	return append(unicode::encode(codepoint));
}

string &string::prepend(std::string_view prefix)
{
	return raw_insert(0, prefix);
}

string &string::prepend(char32_t codepoint)
{
	return prepend(unicode::encode(codepoint));
}

string &string::insert(size_t at, std::string_view other)
{
	auto it = begin();
	std::advance(it, at);

	return raw_insert(it.index(), other);
}

string &string::insert(size_t at, char32_t codepoint)
{
	return insert(at, unicode::encode(codepoint));
}

string &string::raw_insert(size_t at, std::string_view other)
{
	if (other.empty()) {
		return *this;
	}

	//TODO: insertion could be optimized to avoid 1 reallocation when the string is shared.
	size_t new_capacity = raw_size() + other.size() + 1;
	reserve(new_capacity);
	str().insert(at, other.data(), other.size());
	reset_cache();

	return *this;
}

string &string::raw_insert(size_t at, char32_t codepoint)
{
	return raw_insert(at, unicode::encode(codepoint));
}

string &string::remove(std::string_view substring)
{
	return replace(substring, std::string_view());
}

string &string::remove(char32_t codepoint)
{
	return remove(unicode::encode(codepoint));
}

string &string::remove_first(std::string_view substring)
{
	return replace_first(substring, std::string_view());
}

string &string::remove_first(char32_t codepoint)
{
	return remove_first(unicode::encode(codepoint));

}

string &string::remove_last(std::string_view substring)
{
	return replace_last(substring, std::string_view());
}

string &string::remove_last(char32_t codepoint)
{
	return remove_last(unicode::encode(codepoint));
}

string &string::replace(std::string_view before, std::string_view after)
{
	auto current_size = raw_size();

	// Don't waste time if there can't be any match
	if (before.size() > current_size or before.empty()) {
		return *this;
	}

	size_t at = 0;
	size_t limit = current_size - before.size();

	while (at <= limit)
	{
		at = raw_find(before, at);

		if (at != invalid_index())
		{
			raw_replace(at, before.size(), after);
			at += after.size();
		}
	}

	return *this;
}

string &string::replace(size_t index, size_t codepoints, std::string_view after)
{
	auto it = begin();
	std::advance(it, index);
	index = it.index();
	std::advance(it, codepoints);
	codepoints = it.index() - index;

	return raw_replace(index, codepoints, after);
}

string &string::raw_replace(size_t index, size_t nbytes, std::string_view after)
{
	if (nbytes == 0) {
		return raw_insert(index, after);
	}

	size_t new_size = raw_size() - nbytes + after.size();
	reserve(new_size + 1);
	str().replace(index, nbytes, after.data(), after.size());
	reset_cache();

	return *this;
}

string &string::replace_first(std::string_view before, std::string_view after)
{
	auto at = raw_find(before);

	if (at != invalid_index()) {
		raw_replace(at, before.size(), after);
	}

	return *this;
}

string &string::replace_last(std::string_view before, std::string_view after)
{
	auto i = raw_find_last(before);

	if (i != invalid_index()) {
		raw_replace(i, before.size(), after);
	}

	return *this;
}

#ifdef ELVEA_WITH_REGEX
string &string::replace(regex &pattern, const string &after)
{
	if (pattern.match(*this))
	{
		size_t start = pattern.start_offset(0);
		size_t len = pattern.end_offset(0) - start;

		string repl(after);

		// Replace whole match (same as Perl's $&)
		repl.replace("%%", pattern.capture(0));

		// At most 9 captures are replaced
		size_t count = std::min<size_t>(pattern.count(), 9);

		for (size_t i = 1; i <= count; ++i)
		{
			auto num = utils::format("\\%%", i);
			repl.replace(num, pattern.capture(i));
		}

		raw_replace(start, len, repl);
	}

	return *this;
}
#endif // ELVEA_WITH_REGEX

string::iterator string::front()
{
	return begin();
}

string::iterator string::back()
{
	auto it = end();
	return --it;
}

string::const_iterator string::front() const
{
	return cbegin();
}

string::const_iterator string::back() const
{
	auto it = cend();
	return --it;
}

size_t string::length() const
{
	return size();
}

string &string::resize(size_t new_size)
{
	auto current_size = raw_size();

	if (new_size < current_size)
	{
		if (impl->shared())
		{
			string tmp(data(), new_size);
			swap(tmp);
		}
		else
		{
			str().resize(new_size);
			reset_cache();
		}
	}
	else if (new_size > current_size)
	{
		reserve(new_size + 1);
		str().resize(new_size);
		reset_cache();
	}

	return *this;
}

string &string::arg(const string &arg1)
{
	replace("%1", arg1);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2)
{
	replace("%1", arg1);
	replace("%2", arg2);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);
	replace("%5", arg5);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);
	replace("%5", arg5);
	replace("%6", arg6);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);
	replace("%5", arg5);
	replace("%6", arg6);
	replace("%7", arg7);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7, const string &arg8)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);
	replace("%5", arg5);
	replace("%6", arg6);
	replace("%7", arg7);
	replace("%8", arg8);

	return *this;
}

string &string::arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7, const string &arg8, const string &arg9)
{
	replace("%1", arg1);
	replace("%2", arg2);
	replace("%3", arg3);
	replace("%4", arg4);
	replace("%5", arg5);
	replace("%6", arg6);
	replace("%7", arg7);
	replace("%8", arg8);
	replace("%9", arg9);

	return *this;
}


void string::clear()
{
	if (impl->shared())
	{
		if (not empty())
		{
			*this = string();
		}
	}
	else
	{
		// Assume we might want to reuse the buffer.
		str().clear();
		reset_cache();
	}
}

string string::apply(const std::function<char32_t(char32_t)> &callback) const
{
	string result(capacity(), string::allocate());

	for (char32_t codepoint : *this) {
		result.append(callback(codepoint));
	}

	return result;
}

string string::repeat(char32_t codepoint, size_t count)
{
	return repeat(unicode::encode(codepoint), count);
}

string string::repeat(std::string_view s, size_t count)
{
	string result(count * s.size(), string::allocate());

	while (count-- > 0) {
		return result.append(s);
	}

	return result;
}

bool string::to_bool() const
{
	static std::string_view value("false");
	return not empty() and not (*this == value);
}

long string::to_int(bool *ok) const
{
	errno = 0;

	char* c;
	auto value = strtol(data(), &c, 10);
	bool result = (errno == 0) and not (value == 0 and *c != '\0');

	// Only throw an error if ok is null
	if (ok)
	{
		*ok = result;
	}
	else if (!result)
	{
		throw error("Could not convert string to integer");
	}

	return value;
}

double string::to_float(bool *ok) const
{
	errno = 0;
	char* c;
	auto value = strtod(data(), &c);
	bool result = (errno == 0) and not (value == 0.0 and *c != '\0');

	// Only throw an error if ok is null
	if (ok)
	{
		*ok = result;
	}
	else if (! result)
	{
		throw error("Could not convert string to float");
	}

	return value;
}

string &string::trim()
{
	return trim(option::both);
}

string &string::ltrim()
{
	return trim(option::left);
}

string &string::rtrim()
{
	return trim(option::right);
}

std::string_view string::trim(std::string_view s, option flag)
{
	size_t first = 0;
	size_t last = s.size();
	const char *str = s.data();

	if (static_cast<int>(flag) & static_cast<int>(option::left))
	{
		for (first = 0; first < s.size(); ++first)
		{
			if (not isspace(str[first])) {
				break;
			}
		}
	}

	if (static_cast<int>(flag) & static_cast<int>(option::right))
	{
		for (last = s.size(); last > 0; --last)
		{
			if (not isspace(str[last-1])) {
				break;
			}
		}
	}
	size_t len = last - first;

	return { str + first, len };
}

string &string::trim(option flag)
{
	const char *space = " \t\n\r";

	if (static_cast<int>(flag) & static_cast<int>(option::left))
	{
		size_t first = str().find_first_not_of(space);

		// Only white space?
		if (first == std::string::npos)
		{
			unshare();
			str().clear();
			reset_cache();

			return *this;
		}
		else if (first > 0)
		{
			unshare();
			str().erase(0, first);
			reset_cache();
		}
	}

	if (static_cast<int>(flag) & static_cast<int>(option::right))
	{
		size_t last = str().find_last_not_of(space) + 1;

		// Only white space?
		if (last == std::string::npos)
		{
			unshare();
			str().clear();
			reset_cache();

			return *this;
		}
		else if (last < raw_size())
		{
			unshare();
			const size_t len = raw_size() - last;
			str().erase(last, len);
			reset_cache();
		}
	}

	return *this;
}

std::vector<char32_t> string::codepoints() const
{
	std::vector<char32_t> result;

	for (char32_t codepoint : *this) {
		result.push_back(codepoint);
	}

	return result;
}

string string::join(const std::vector<string> &strings, std::string_view separator)
{
	string result;
	size_t size = 1;

	for (auto &s : strings) {
		size += s.raw_size() + separator.size();
	}

	result.reserve(size);
	auto count = strings.size();

	for (size_t i = 0; i < count; i++)
	{
		result.append(strings[i]);

		if (i < count - 1) {
			result.append(separator);
		}
	}

	return result;
}

std::vector<string> string::split(std::string_view separator) const
{
	std::vector<string> strings;
	size_t substring_start;
	size_t string_last; /* end position accounting for the size of the splitter */
	size_t len;
	const char *substring, *current_chunk;

	/* The splitter needs to be longer than the string to be split */
	if (separator.size() >= raw_size())
	{
		strings.emplace_back(*this);
		return strings;
	}

	substring_start = 0;
	string_last = raw_size() - separator.size();

	for (size_t i = 0; i <= string_last; ++i)
	{
		current_chunk = data() + i;

		if (strncmp(current_chunk, separator.data(), separator.size()) == 0)
		{
			/* Get the length of the current unmatched text */
			len = i - substring_start;

			/* Get pointer to current substring */
			substring = data() + substring_start;

			/* Add string to string list */
			strings.emplace_back(string(substring, len));

			/* Skip the replaced chunk */
			substring_start = i + separator.size();
			i = substring_start - 1;
		}
	}

	/* Get right substring */
	if (substring_start == this->raw_size()) /* The end of the string matched the splitter */
	{
		strings.emplace_back(string());
	}
	else
	{
		len = raw_size() - substring_start;
		substring = data() + substring_start;
		strings.emplace_back(string(substring, len));
	}

	return strings;
}

size_t string::count(std::string_view substring) const
{
	return count(to_string_view(), substring);
}

size_t string::count(char32_t codepoint) const
{
	return count(unicode::encode(codepoint));
}

size_t string::count(std::string_view self, std::string_view substring)
{
	size_t result = 0;
	size_t pos = 0;

	while (pos < self.size())
	{
		pos = raw_find(self, substring, pos);

		if (pos != invalid_index())
		{
			result++;
			pos += substring.size();
		}
	}

	return result;
}

void string::check_range(size_t pos) const
{
	if (pos > raw_size())
	{
		throw error("Cannot access code point starting at index % in string with size %", pos, raw_size());
	}
}

string::const_iterator string::at(size_t pos) const
{
	auto it = get(pos);
	check_range(it.index());

	return it;
}

string::iterator string::at(size_t pos)
{
	auto it = get(pos);
	check_range(it.index());

	return it;
}

string::iterator string::raw_at(size_t pos)
{
	check_range(pos);
	return raw_get(pos);
}

string::const_iterator string::raw_at(size_t pos) const
{
	check_range(pos);
	return raw_get(pos);
}

string::iterator string::get(size_t pos)
{
	auto it = begin();
	std::advance(it, pos);

	return it;
}

string::const_iterator string::get(size_t pos) const
{
	auto it = cbegin();
	std::advance(it, pos);

	return it;
}

string::iterator string::operator[](size_t pos)
{
	return get(pos);
}

string::const_iterator string::operator[](size_t pos) const
{
	return get(pos);
}

string::iterator string::raw_get(size_t pos)
{
	return iterator(*this, pos);
}

string::const_iterator string::raw_get(size_t pos) const
{
	return iterator(*this, pos);
}

string &string::fill(char32_t codepoint, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		append(codepoint);
	}

    return *this;
}

std::string_view string::to_string_view() const
{
	return { data(), raw_size() };
}

size_t string::use_count() const
{
	return impl->use_count();
}

string string::operator+(const string &other)
{
	string tmp(*this);
	return tmp.append(other);
}

string string::operator+(std::string_view &other)
{
	string tmp(*this);
	return tmp.append(other);
}

string &string::operator+=(const string &other)
{
	return append(other);
}

string &string::operator+=(std::string_view &other)
{
	return append(other);
}

bool string::valid() const
{
	return unicode::is_legal_utf8(data(), raw_size());
}


} // runtime
