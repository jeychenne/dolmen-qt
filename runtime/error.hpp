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
 * Created: 2017.08.04                                                                                                 *
 *                                                                                                                     *
 * Purpose: base class for all elvea exceptions.                                                                       *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_ERROR_HPP
#define ELVEA_ERROR_HPP

#include <stdexcept>
#include "utils/print.hpp"

namespace runtime {

class error : public std::runtime_error
{
public:

	template<typename T, typename... Args>
	error(const char* fmt, const T &value, Args... args) :
			error(utils::format(fmt, value, args...))
	{ }

	error(const std::string &msg) :
			std::runtime_error(msg)
	{ }

	error(const char *msg) :
			std::runtime_error(msg)
	{ }
};

} // runtime

#endif // ELVEA_ERROR_HPP
