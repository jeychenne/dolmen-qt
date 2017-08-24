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
 * Purpose: helper routines to add Unicode support to Lua.                                                             *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef DOLMEN_HELPERS_H
#define DOLMEN_HELPERS_H

#include <cstddef>
#include <cstdio>
#include <string>
#include <cassert>

#include <QString>



namespace utils {


// Portable wrapper over fopen/_wfopen.
FILE *open_file(const std::string &path, const char *mode);

// Portable wrapper over freopen/_wfreopen.
FILE *reopen_file(const std::string &path, const char *mode, FILE *handle);

QString error_message();

void print(const char *msg, ...);

void fprint(FILE *file, const char *msg, ...);

} // namespace utils

#endif //DOLMEN_HELPERS_H
