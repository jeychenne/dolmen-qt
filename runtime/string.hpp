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
 * Purpose: A UTF-8, NULL-terminated string with copy-on-write semantics built on top of std::string.                  *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_STRING_HPP
#define ELVEA_STRING_HPP

#include <cstring>
#include <string>
#include <vector>

#include "error.hpp"
#include "utils/ref_count.hpp"

#include <string_view>

#ifdef __APPLE__
#include <string_view>
#elif __cplusplus > 201402L
#include <string_view>
#elif defined(Q_OS_UNIX)
#else
#include "extra/string_view.hpp"

namespace std {
using bpstd::string_view; // until C++17 becomes widely available
}
#endif


namespace runtime {


namespace traits {

template<typename T>
T from_string(const std::string &s);

template<>
inline int from_string<int>(const std::string &s)
{
	return std::stoi(s);
}


template<>
inline long from_string<long>(const std::string &s)
{
	return std::stol(s);
}

template<>
inline double from_string<double>(const std::string &s)
{
	return std::stod(s);
}

template<>
inline bool from_string<bool>(const std::string &s)
{
	bool b;
	std::istringstream(s) >> std::boolalpha >> b;

	return b;
}

} // traits


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class regex;


class string final
{
public:

	static const size_t npos = std::string::npos;

	// Forward declaration
	class iterator;

	// Helper class for iterators
	class codepoint_ref
	{
	public:

		explicit codepoint_ref(iterator *it) : str(it->str), pos(it->pos)
		{ }

		~codepoint_ref() = default;

		operator char32_t() const;
		codepoint_ref& operator=(char32_t cp);
		codepoint_ref& operator=(const iterator &it);
		codepoint_ref& operator=(const codepoint_ref &other);

		bool operator<(const codepoint_ref &other) const;
		bool operator==(const codepoint_ref &other) const;
		bool operator!=(const codepoint_ref &other) const { return not (*this == other); }

	private:
		const string *str;
		size_t pos;
	};

	// Iterates over UTF-8 code points.
	// To iterate backward, use something like this
	// for (auto it = s1.end(); it != s1.begin();) {
	//     --it;
	//     ...
	// }

	class iterator : public std::iterator<std::forward_iterator_tag, char32_t, size_t>
	{
	public:

		explicit iterator(const string &s, size_t pos = 0);

		iterator(const iterator &other);

		~iterator() = default;

		iterator &operator=(const iterator &other);

		// Pre-increment.
		iterator &operator++();

		// Post-increment.
		iterator operator++(int);

		// Pre-decrement.
		iterator &operator--();

		// Post-decrement.
		iterator operator--(int);

		bool operator==(const iterator &other) const;

		bool operator!=(const iterator &other) const;

		iterator operator+(int i) const;
//		iterator operator-(int i) const;

		iterator &operator=(char32_t);

		codepoint_ref operator*() const;

		operator char32_t() const;

		size_t index() const { return pos; }

		char32_t codepoint() const;

	private:

		friend class codepoint_ref;

		const string *str;
		size_t pos;

		void assign(char32_t);
	};

	typedef const iterator const_iterator;
	typedef char32_t value_type;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	// Flags for trim()
	enum class option {
		left  = 1,
		right = 2,
		both  = 3
	};

	// Flag to dispatch to the constructor which only allocates a buffer of a given size.
	enum class allocate {};


	// Create a 0-initialized empty string.
	string();

	// Copy constructor.
	string(const string &other) noexcept ;

	// Move constructor.
	string(string &&other) noexcept;

	// Destructor.
	~string();

	// Construct from a null-terminated C string.
	string(const char *str);

	// Construct from a C string with an explicit size.
	string(const char *str, size_t len);

	string(std::string_view str);

	string(const std::string &str);

	// Allocated an empty string with a buffer at least as large as requested.
	string(size_t requested, allocate);

	// Construct from a UTF-8 code point.
	explicit string(char32_t codepoint);

	// Get the underlying buffer. Contrary to what the C++ standard mandates, this is not necessarily null-terminated.
	const char *data() const { return str().data(); }

	size_t size() const;
	size_t length() const;

	// Get number of bytes in the string.
	size_t raw_size() const {
		return impl->text.size();
	}

	bool empty() const {
		return raw_size() == 0;
	}

	size_t capacity() const;

	void reserve(size_t requested);

    static size_t max_size();
    static size_t invalid_index();

	string &operator=(const string &other);
	string &operator=(string &&other) noexcept;

	void swap(string &other) noexcept;

	string clone() const;

	std::string &str() {
		return impl->text;
	}

	const std::string &str() const {
		return impl->text;
	}

	operator std::string() const {
		return impl->text;
	}

	operator const char*() const {
		return data();
	}

	operator std::string_view() const {
		return to_string_view();
	}

	std::string_view to_string_view() const;

	bool operator==(const string &other) const;
	bool operator==(const std::string &other) const;
	bool operator==(std::string_view other) const;
	bool operator==(const char *str) const;
	bool operator==(char32_t codepoint) const;

	bool operator!=(const string &other) const;
	bool operator!=(const std::string &other) const;
	bool operator!=(std::string_view other) const;
	bool operator!=(const char *str) const;
	bool operator!=(char32_t codepoint) const;

	bool operator<(const string &other) const;
	bool operator<(const std::string &other) const;
	bool operator<(std::string_view other) const;
	bool operator<(const char *str) const;
	bool operator<(char32_t codepoint) const;

	bool operator>(const string &other) const;
	bool operator>(const std::string &other) const;
	bool operator>(std::string_view other) const;
	bool operator>(const char *str) const;
	bool operator>(char32_t codepoint) const;

	bool operator<=(const string &other) const;
	bool operator<=(const std::string &other) const;
	bool operator<=(std::string_view other) const;
	bool operator<=(const char *str) const;
	bool operator<=(char32_t codepoint) const;

	bool operator>=(const string &other) const;
	bool operator>=(const std::string &other) const;
	bool operator>=(std::string_view other) const;
	bool operator>=(const char *str) const;
	bool operator>=(char32_t codepoint) const;

	int compare(const string &other) const;
	int compare(const std::string &other) const;
	int compare(std::string_view other) const;
	int compare(const char *str) const;
	int compare(char32_t codepoint) const;

	string operator+(const string &other);
	string operator+(std::string_view &other);
	string &operator+=(const string &other);
	string &operator+=(std::string_view &other);

	// Get the code point index starting at `pos`
	size_t index(size_t pos) const;

	// Get the code unit index starting at code point `pos`
	size_t raw_index(size_t pos) const;

	bool starts_with(std::string_view prefix) const;
	bool starts_with(char32_t codepoint) const;
	static bool starts_with(std::string_view self, std::string_view prefix);

	bool ends_with(std::string_view suffix) const;
	bool ends_with(char32_t codepoint) const;
	static bool ends_with(std::string_view self, std::string_view suffix);

	bool contains(std::string_view substring) const;
	bool contains(char32_t codepoint) const;
	static bool contains(std::string_view self, std::string_view substring);

	/**
	 * Return the index of a substring in the string.
	 * @param substring substring to find
	 * @param from start position, as a byte offset.
	 * @return position of the substring, or `npos` if it is not found.
	 */
	size_t raw_find(std::string_view substring, size_t from = 0) const;
	size_t raw_find(const char *substring,  size_t len, size_t from = 0) const;
	size_t raw_find(char32_t codepoint, size_t from = 0) const;
	static size_t raw_find(std::string_view self, std::string_view substring, size_t from = 0);

	size_t find(std::string_view substring, size_t from = 0, bool raw_result = false) const;
	size_t find(const char *substring, size_t len, size_t from = 0, bool raw_result = false) const;
	size_t find(char32_t codepoint, size_t from = 0, bool raw_result = false) const;

	size_t raw_find_last(std::string_view substring, size_t to = npos) const;
	size_t raw_find_last(const char *substring, size_t len, size_t to = npos) const;
	size_t raw_find_last(char32_t codepoint, size_t to = npos) const;

	size_t find_last(std::string_view substring, size_t to = npos, bool raw_result = false) const;
	size_t find_last(const char *substring, size_t len, size_t to = npos, bool raw_result = false) const;
	size_t find_last(char32_t codepoint, size_t to = npos, bool raw_result = false) const;

	// Extract substring from position `from` (inclusive) up to `to` (exclusive)
	string raw_substr(size_t from, size_t to = invalid_index()) const;
	string substr(size_t from, size_t to = invalid_index()) const;

	// Get the count first/last codepoints in a string.
	string left(size_t count = 1) const;
	string right(size_t count = 1) const;

	iterator front();
	iterator back();
	const_iterator front() const;
	const_iterator back() const;

	string reverse() const;

	string to_upper() const;
	string to_lower() const;

	bool is_upper() const;
	bool is_lower() const;

	string &append(std::string_view suffix);
	string &append(char32_t codepoint);

	string &prepend(std::string_view prefix);
	string &prepend(char32_t codepoint);

	string &insert(size_t at, std::string_view other);
	string &insert(size_t at, char32_t codepoint);
	string &raw_insert(size_t at, std::string_view other);
	string &raw_insert(size_t at, char32_t codepoint);

	string &remove(std::string_view substring);
	string &remove(char32_t codepoint);

	string &remove_first(std::string_view substring);
	string &remove_first(char32_t codepoint);

	string &remove_last(std::string_view substring);
	string &remove_last(char32_t codepoint);

	string &replace(std::string_view before, std::string_view after);
	string &replace(size_t index, size_t codepoints, std::string_view after);
	string &raw_replace(size_t index, size_t nbytes, std::string_view after);

	string &replace_first(std::string_view before, std::string_view after);
	string &replace_last(std::string_view before, std::string_view after);

	#ifdef ELVEA_WITH_REGEX
	string &replace(regex &pattern, const string &after);
	#endif

	// Resizes the string. If the new size exceeds the current size, the content of the string is undefined.
	string &resize(size_t new_size);

	void clear();

	// Replace positional arguments %1 to %9. This is similar to Qt's QString::arg method.
	string &arg(const string &arg1);
	string &arg(const string &arg1, const string &arg2);
	string &arg(const string &arg1, const string &arg2, const string &arg3);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7, const string &arg8);
	string &arg(const string &arg1, const string &arg2, const string &arg3, const string &arg4, const string &arg5, const string &arg6, const string &arg7, const string &arg8, const string &arg9);

	string apply(const std::function<char32_t(char32_t)> &callback) const;

	static string repeat(char32_t codepoint, size_t count);
	static string repeat(std::string_view s, size_t count);

	// Convert a value (int, bool, double, etc.) to a string.
	template<typename T>
	static string from(const T &value)
	{
		return utils::format("%", value);
	}

	bool to_bool() const;
	long to_int(bool *ok = nullptr) const;
	double to_float(bool *ok = nullptr) const;


	string & trim();
	string & ltrim();
	string & rtrim();
	static std::string_view trim(std::string_view, option flag);

	std::vector<char32_t> codepoints() const;
	std::vector<string> split(std::string_view separator) const;
	static string join(const std::vector<string> &strings, std::string_view separator = " ");

	size_t count(std::string_view substring) const;
	size_t count(char32_t codepoint) const;
	static size_t count(std::string_view self, std::string_view substring);

	// Access code point or code unit, throwing an out-of-range exception if index is invalid
	iterator at(size_t pos);
	const_iterator at(size_t pos) const;
	iterator raw_at(size_t pos);
	const_iterator raw_at(size_t pos) const;

	// Same as at(), but doesn't check range
	iterator get(size_t pos);
	const_iterator get(size_t pos) const;
	iterator raw_get(size_t pos);
	const_iterator raw_get(size_t pos) const;

	// Same as get, for UTF-8 code points
	iterator operator[](size_t pos);
	const_iterator operator[](size_t pos) const;


	string &fill(char32_t codepoint, size_t count);

	// For debugging...
	size_t use_count() const;

	// Is it valid UTF-8?
	bool valid() const;

	template<typename T>
	T as(bool *ok = nullptr) const;

private:

	friend class iterator;

	struct storage : shared_storage<storage>
	{
		storage() = default;
		
		storage(const char *s, size_t len) :
				text(s, len)
		{ }
		
		explicit storage(size_t capacity)
		{ text.reserve(capacity); }
		
		~storage() = default;

		// Cache for Unicode size (number of code points).
		size_t utf8_len = npos;

		// Null-terminated array of bytes.
		std::string text;
	};

	storage *impl;

	void retain() {
		if (impl) impl->retain();
	}

	void release() {
		if (impl) impl->release();
	}

	void invalidate() {
		impl = nullptr;
	}

	bool equal(const char *str, size_t len) const;
	bool not_equal(const char *str, size_t len) const;
	bool less_than(const char *str) const;
	bool less_equal(const char *str) const;
	bool greater_than(const char *str) const;
	bool greater_equal(const char *str) const;

	// Does this string have a shared storage?
	bool shared() const {
		return impl->shared();
	}

	// Ensure the string is unique.
	void unshare();

	void reset_cache() {
		impl->utf8_len = npos;
	}

	string & trim(option flag);

	void check_range(size_t pos) const;

	storage *empty_string() {
		static storage e;
		return &e;
	}
};


//----------------------------------------------------------------------------------------------------------------------

template<typename T>
inline T string::as(bool *ok) const
{
	static_assert(std::is_default_constructible<T>::value, "T must be default-constructible");

	if (ok != nullptr)
	{
		try {
			auto i = traits::from_string<T>(str());
			*ok = true;

			return i;
		}
		catch (...)
		{
			*ok = false;
			return T();
		}
	}
	else
	{
		return traits::from_string<T>(str()); // let exceptions propagate
	}
}



} // runtime


#endif // ELVEA_STRING_HPP
