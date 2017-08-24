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
 * Purpose: base for reference counted heap allocated structure.                                                       *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_REFCOUNT_HPP
#define ELVEA_REFCOUNT_HPP

#include <atomic>
#include <memory>

namespace runtime {


template<typename T>
class shared_storage
{
public:

	shared_storage() : ref_count(1)
	{ }

	void retain()
	{
		ref_count.fetch_add(1, std::memory_order_relaxed);
	}

	void release()
	{
		if (ref_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
			delete static_cast<T*>(this);
		}
	}

	void add_reference() {
		ref_count.fetch_add(1, std::memory_order_relaxed);
	}

	size_t remove_reference() {
		return ref_count.fetch_sub(1, std::memory_order_relaxed);
	}

	bool shared() const
	{
		return ref_count.load(std::memory_order_relaxed) > 1;
	}

	size_t use_count() const
	{
		return ref_count.load(std::memory_order_relaxed);
	}

protected:

	std::atomic<size_t> ref_count;
};



} // runtime

#endif // ELVEA_REFCOUNT_HPP
