/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2010-2017 Julien Eychenne                                                                             *
 *                                                                                                                     *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not   *
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.                                      *
 *                                                                                                                     *
 * Software distributed under this License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either    *
 * express or implied. See the License for the specific language governing rights and limitations under the License.   *
 *                                                                                                                     *
 * Created: 2017.07.08                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <ctime>
#include <cstdarg>
#include <random>
#include <sstream>
#include <iostream>
#include <codecvt>
#include <locale>

#ifdef _WIN32
	#include <wchar.h>
#else
    #include <errno.h>
#include <cstring>

#endif

#include "helpers.h"

#if Q_OS_WINDOWS
#include "runtime/unicode.hpp"
using namespace runtime;
using namespace runtime::unicode;
#endif

namespace utils {

FILE *open_file(const std::string &path, const char *mode)
{
	FILE *handle;

#ifdef WIN32
    std::u16string wpath(utf8_to_utf16(path));
    std::u16string wmode(utf8_to_utf16(mode));
	auto wpath_data = reinterpret_cast<const wchar_t*>(wpath.data());
	auto wmode_data = reinterpret_cast<const wchar_t*>(wmode.data());
	handle = _wfopen(wpath_data, wmode_data);
#else
	handle = fopen(path.data(), mode);
#endif

	if (handle == nullptr) {
		std::string msg("File not found: ");
		msg.append(path);

        throw std::runtime_error(msg);
	}

	return handle;
}

FILE *reopen_file(const std::string &path, const char *mode, FILE *handle)
{
#ifdef WIN32
    std::u16string wpath(utf8_to_utf16(path));
    std::u16string wmode(utf8_to_utf16(mode));
	auto wpath_data = reinterpret_cast<const wchar_t*>(wpath.data());
	auto wmode_data = reinterpret_cast<const wchar_t*>(wmode.data());
	handle = _wfreopen(wpath_data, wmode_data, handle);
#else
	handle = freopen(path.data(), mode, handle);
#endif

	if (handle == nullptr) {
		std::string msg("Cannot reopen file: ");
		msg.append(path);

        throw std::runtime_error(msg);
	}

	return handle;
}

QString error_message()
{
#ifdef Q_OS_WINDOWS
	DWORD err = GetLastError();
	LPTSTR msg = NULL;

	FormatMessage(
	   FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
	   NULL,
	   err,
	   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	   (LPTSTR)&msg,
	   0,
	   NULL);

	if (msg != NULL)
	{
		std::u16string utf16((char16_t*)msg, wcslen(msg));
        String s = utf16_to_utf8(utf16);
		LocalFree((HLOCAL)msg);

		return s;
	}
	else {
		return String();
	}
#else
	return strerror(errno);
#endif
}

static void print_string(FILE *file, const char *ln)
{
#ifdef Q_OS_WINDOWS
	String line(ln);
    auto utf16 = utf8_to_utf16(line);
	fwprintf(file, L"%s", (wchar_t*)utf16.data());
#else
	fprintf(file, "%s", ln);
#endif
}

void print(const char *msg, ...)
{
	char dest[512];
	va_list args;

	va_start(args, msg);
	vsprintf(dest, msg, args);
	va_end(args);

	print_string(stdout, dest);
}

void fprint(FILE *file, const char *msg, ...)
{
	char dest[512];
	va_list args;

	va_start(args, msg);
	vsprintf(dest, msg, args);
	va_end(args);

	print_string(file, dest);
}

} // namespace utils
