/******************************************************************//**
 * \file   common.hpp
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
 *********************************************************************/

#ifndef INCLUDE_GUARD_DFBD8064_93B7_4262_9B4C_AB9046ADF2E2
#define INCLUDE_GUARD_DFBD8064_93B7_4262_9B4C_AB9046ADF2E2


#include "string_view.hpp"

#include <cassert>
#include <vector>

#if __cplusplus >= 201103L || _MSC_VER >= 1800
  #define SAXY_CPP11 1
#endif

#if defined __GNUG__
#define UNREACHABLE __builtin_unreachable()
#elif defined _MSC_VER
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE (void*)0
#endif

#define SAXY_CHANGE_STATE(X) m_state = X; goto X
#define SAXY_RESTART_STATE(X) goto X
#define SAXY_CHANGE_STATE_AFTER(X, Y) m_state = X; do {Y;} while(false); goto X
#define SAXY_STATE_JUMP_TABLE(X) case X: goto X
#define SAXY_RUN_CALLBACK(X) \
{ \
	command c = X; \
	if(c != keep_going) { \
		return detail::to_return_value(c); \
	} \
}

namespace saxy {

template <char State>
struct state {
	static char const value = State;
};

typedef state<0> always_keep_going;
typedef state<1> always_stop;
typedef state<2> always_abort;

inline
void require_abort(always_abort) {
}

static always_keep_going keep_going;
static always_stop stop;
static always_abort abort;

template <char State1, char State2>
bool operator==(state<State1>, state<State2>) {
	return State1 == State2;
}

template <char State1, char State2>
bool operator!=(state<State1>, state<State2>) {
	return State1 != State2;
}

class command {
	char m_state;

public:
	template <char State>
	command(state<State>)
	: m_state(State) {
	}

	friend bool operator==(command lhs, command rhs) {
		return lhs.m_state == rhs.m_state;
	}

	friend bool operator!=(command lhs, command rhs) {
		return lhs.m_state != rhs.m_state;
	}

	template <char State>
	friend bool operator==(command lhs, state<State>) {
		return lhs.m_state == State;
	}

	template <char State>
	friend bool operator==(state<State>, command rhs) {
		return rhs.m_state == State;
	}

	template <char State>
	friend bool operator!=(command lhs, state<State>) {
		return lhs.m_state != State;
	}

	template <char State>
	friend bool operator!=(state<State>, command rhs) {
		return rhs.m_state != State;
	}
};

namespace detail {

struct simd {};
struct no_simd {};

template <typename It, typename EndIt>
struct use_simd {
	typedef no_simd type;
};

template <>
struct use_simd<char*, char*> {
	typedef simd type;
};

template <>
struct use_simd<char const*, char*> {
	typedef simd type;
};

template <>
struct use_simd<char*, char const*> {
	typedef simd type;
};

template <>
struct use_simd<char const*, char const*> {
	typedef simd type;
};

struct cstr_end_iterator {

};

inline
bool operator==(cstr_end_iterator, char const* it) {
	return *it == '\0';
}

inline
bool operator==(char const* it, cstr_end_iterator) {
	return *it == '\0';
}

inline
bool operator!=(cstr_end_iterator, char const* it) {
	return *it != '\0';
}

inline
bool operator!=(char const* it, cstr_end_iterator) {
	return *it != '\0';
}

template <typename Container>
class scope_clear {
	Container& m_cont;

public:
	scope_clear(Container& cont)
	: m_cont(cont) {
	}

	~scope_clear() {
		m_cont.clear();
	}
};

template <typename T>
class scope_assign {
	const T& m_from;
	T* m_to;

public:
	scope_assign(const T& from, T* to)
	: m_from(from)
	, m_to(to) {
	}

	~scope_assign() {
		if(m_to) {
			*m_to = m_from;
		}
	}
};

inline
bool to_return_value(command c) {
	assert(c != keep_going);
	return c == stop;
}

template <typename Vector>
class append_to_vector {
	Vector* m_container;

public:
	append_to_vector(Vector& container)
	: m_container(&container) {
	}

	void start(char const*) const {
		m_container->push_back('\0');
	}

	void append(char ch) {
		m_container->insert(m_container->end() - 1, ch);
	}

	template <typename It>
	void append(It begin, It end) {
		m_container->insert(m_container->end() - 1, begin, end);
	}

	void append_same(char ch) {
		m_container->insert(m_container->end() - 1, ch);
	}

	template <typename It>
	void append_same(It begin, It end) {
		m_container->insert(m_container->end() - 1, begin, end);
	}

	void clear() {
		m_container->clear();
	}

	string_cview view_string() {
		return string_cview(m_container->data(), m_container->size() - 1);
	}
};

template <typename Vector>
append_to_vector<Vector> make_dis(Vector& v) {
	return append_to_vector<Vector>(v);
}

class in_place {
	char* m_start;
	char* m_current;

public:
	in_place(char* start)
	: m_start(start)
	, m_current(start) {
	}

	char* start_pos() {
		return m_start;
	}

	char const* start_pos() const {
		return m_start;
	}

	char* current_pos() {
		return m_current;
	}

	char const* current_pos() const {
		return m_current;
	}

	void start(char* str) {
		m_start = str;
		m_current = str;
	}

	void append(char ch) {
		*m_current++ = ch;
	}

	template <typename It>
	void append(It begin, It end) {
		while(begin != end) {
			*m_current++ = *begin++;
		}
	}

	void append_same(char) {
		m_current++;
	}

	template <typename It>
	void append_same(It begin, It end) {
		m_current += (end - begin);
	}

	void clear() const {
	}

	string_view view_string() const {
		return string_view(m_start, m_current - m_start);
	}
};

/** Returns the number of leading 0 bits in \a data. */
__forceinline int count_leading_zeros(unsigned data) {
#ifdef _MSC_VER
	unsigned long index;
	if(_BitScanForward(&index, data)) {
		return index;
	} else {
		return 16;
	}
#else
	if(data != 0) {
		return __builtin_ctz(data);
	} else {
		return 16;
	}
#endif
}

}

}

#endif
