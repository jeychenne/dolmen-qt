//
// Created by julien on 05/08/17.
//

#ifndef ELVEA_PRINT_HPP
#define ELVEA_PRINT_HPP

#include <sstream>


namespace runtime { namespace utils {

namespace detail {

inline
void format(std::ostringstream &msg, const char *fmt)
{
	msg << fmt;
}

template<typename T, typename... Args>
void format(std::ostringstream &msg, const char* fmt, const T &value, Args... args)
{
	for (; *fmt != '\0'; fmt++)
	{
		if (*fmt == '\\' and *(fmt+1) == '%')
		{
			msg << "%";
			format(msg, fmt + 2, value, args...);
			return;
		}
		if (*fmt == '%')
		{
			msg << value;
			format(msg, fmt + 1, args...);
			return;
		}
		msg << *fmt;
	}
}

// Cross platform printing function. On Windows, the string is converted to UTF-16 and printed using the OS's native
// function.
void print(FILE *file, const std::string &s);

} // detail


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Replace each occurrence of '%' in a format string with a formatting argument. To print "%", use "\\%"
// Adapted from http://en.cppreference.com/w/cpp/language/parameter_pack
template<typename T, typename... Args>
std::string format(const char* fmt, const T &value, Args... args)
{
	std::ostringstream stream;
	detail::format(stream, fmt, value, args...);

	return stream.str();
}


// Similar to std::printf, but uses format() to format arguments and detail::print() to print them.
template<typename T, typename... Args>
void printf(const char* fmt, T value, Args... args)
{
	detail::print(stdout, format(fmt, value, args...));
};

// Similar to std::fprintf, but uses format() to format arguments and detail::print() to print them.
template<typename T, typename... Args>
void fprintf(FILE *file, const char* fmt, T value, Args... args)
{
	detail::print(file, format(fmt, value, args...));
};


} // utils
} // runtime

#endif // ELVEA_PRINT_HPP
