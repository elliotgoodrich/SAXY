/*************************************************************************//**
 * \file   json.hpp
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

#ifndef INCLUDE_GUARD_CF7BF358_ECA4_441C_9585_9E7224FE5F70
#define INCLUDE_GUARD_CF7BF358_ECA4_441C_9585_9E7224FE5F70

#include "common.hpp"

#include <cassert>
#include <string>

namespace saxy {

class json {
	enum state {
		begin,
	};

public:
	static char const* name; ///< "JSON"

	enum error_code {
		none = 0, ///< No error
	};

	enum event {
		begin_array_event,
		end_array_event,
		begin_object_event,
		end_object_event,
		name_event,
		number_event,
		string_event,
		boolean_event,
		null_event,
		error_event      ///<
	};

	template <typename Callback, typename Return = command>
	class event_callback {
		Callback* m_cb;

	public:
		explicit event_callback(Callback& cb)
		: m_cb(&cb) {
		}

		Return begin_array() {
			return m_cb->event(begin_array_event, string_view());
		}

		Return end_array() {
			return m_cb->event(end_array_event, string_view());
		}

		Return begin_object() {
			return m_cb->event(begin_object_event, string_view());
		}

		Return end_object() {
			return m_cb->event(end_object_event, string_view());
		}

		template <typename StringView>
		Return name(StringView str) {
			return m_cb->event(name_event, str);
		}

		template <typename StringView>
		Return number(StringView str) {
			return m_cb->event(number_event, str);
		}

		template <typename StringView>
		Return string(StringView str) {
			return m_cb->event(string_event, str);
		}

		Return boolean(bool b) {
			return m_cb->event(string_event, str);
		}

		always_abort error(error_code code) {
			return m_cb->error(code);
		}
	};
};

}

#endif
