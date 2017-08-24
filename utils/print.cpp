//
// Created by julien on 05/08/17.
//


#include "print.hpp"
#include "runtime/unicode.hpp"

namespace runtime { namespace utils { namespace detail {

void print(FILE *file, const std::string &s)
{
#ifdef ELVEA_WINDOWS
	auto utf16 = unicode::utf8_to_utf16(s);
	fwprintf(file, L"%s", (wchar_t*)utf16.data());
#else
	std::fprintf(file, "%s", s.data());
#endif
}

}}} // runtime::utils::detail
