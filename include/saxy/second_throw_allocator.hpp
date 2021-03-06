/*************************************************************************//**
 * \file   second_throw_allocator.hpp
 * \author Elliot Goodrich
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef INCLUDE_GUARD_0ABC26C6_D2E4_40A2_BBF0_D79785E77A26
#define INCLUDE_GUARD_0ABC26C6_D2E4_40A2_BBF0_D79785E77A26

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace saxy {

template <typename T>
class second_throw_allocator : public std::allocator<T> {
	std::size_t m_alloc_count;

	template <typename U>
	friend class second_throw_allocator;

public:
	typedef second_throw_allocator<T> other;

	typedef typename std::allocator<T>::value_type value_type;
	typedef typename std::allocator<T>::pointer pointer;
	typedef typename std::allocator<T>::const_pointer const_pointer;
	typedef typename std::allocator<T>::reference reference;
	typedef typename std::allocator<T>::const_reference const_reference;
	typedef typename std::allocator<T>::size_type size_type;
	typedef typename std::allocator<T>::difference_type difference_type;
	typedef typename std::true_type propagate_on_container_copy_assignment;
	typedef typename std::true_type propagate_on_container_move_assignment;
	typedef typename std::true_type propagate_on_container_swap;

	second_throw_allocator()
	: m_alloc_count(0) {
	}

	template <class U>
	second_throw_allocator(second_throw_allocator<U> const& other)
	: m_alloc_count(other.m_alloc_count) {
	}

	pointer allocate(size_type n, typename std::allocator<void>::const_pointer hint = 0) {
		if(++m_alloc_count == 2) {
			throw std::bad_alloc();
		}

		return std::allocator<T>::allocate(n, hint);
	}

	template <typename Other>
	struct rebind {
		typedef second_throw_allocator<Other> other;
	};
};

}

#endif