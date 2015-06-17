#include "catch/catch.hpp"

#include "saxy/csv.hpp"
#include "saxy/second_throw_allocator.hpp"
#include "saxy/stack_allocator.hpp"

#include "util/tools.hpp"

#ifdef SAXY_CPP11
#include <functional>
#endif
#include <stdexcept>
#include <string>
#include <vector>

struct csv_test_parser {
	enum what_to_do {
		exception,
		stop,
		abort,
	};

	std::string xml;
	saxy::csv::error_code csv_error;
	int error_count;
	int throw_at;
	what_to_do response;
	int function_calls;

	csv_test_parser()
	: csv_error(saxy::csv::none)
	, error_count(0)
	, throw_at(-1)
	, function_calls(0) {
	}

	csv_test_parser(what_to_do what, int i)
	: csv_error(saxy::csv::none)
	, error_count(0)
	, throw_at(i)
	, response(what)
	, function_calls(0) {
	}

	saxy::command return_helper() {
		if(function_calls++ == throw_at) {
			if(response == stop) {
				return saxy::stop;
			} else if(response == abort) {
				return saxy::abort;
			} else if(response == exception) {
				throw std::runtime_error("");
			}
		}

		return saxy::keep_going;
	}

	saxy::command start_row() {
		xml += '{';
		return return_helper();
	}

	saxy::command end_row() {
		xml += '}';
		return return_helper();
	}

	saxy::command field(saxy::string_cview str) {
		xml += '[';
		xml.append(str.data(), str.size());
		xml += ']';
		return return_helper();
	}

	saxy::always_abort error(saxy::csv::error_code e) {
		++error_count;
		csv_error = e;
		return_helper();
		return saxy::abort;
	}
};

void check_conversion(int line, std::string const& csv, std::string const& xml) {
	// Check that conversion works
	{
		INFO("Testing static conversion");
		INFO("Line: " << line);
		std::vector<char> copy(csv.begin(), csv.end());
		csv_test_parser converter;
		CHECK(saxy::csv::parse(converter, copy.data(), copy.size()));
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check that conversion works
	{
		INFO("Testing static conversion and finish");
		INFO("Line: " << line);
		std::vector<char> copy(csv.begin(), csv.end() - 2);
		csv_test_parser converter;
		CHECK(saxy::csv::parse(converter, copy.data(), copy.size()));
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check that partial conversion works
	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		INFO("Testing statefulness");
		INFO("Line: " << line << ", i = " << i);
		std::string::const_iterator middle = csv.begin() + i;
		CHECK(parser.parse(converter, csv.begin(), middle));
		CHECK(parser.parse(converter, middle, csv.end()));
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		std::vector<char> copy(csv.begin(), csv.end());
		csv_test_parser converter;
		saxy::csv::in_place_parser parser(copy.data(), copy.size());
		INFO("Testing statefulness for in place");
		INFO("Line: " << line << ", i = " << i);
		CHECK(parser.parse(converter, i));
		CHECK(parser.parse(converter));
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check exception safety when buffer throws
	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		csv_test_parser converter;
		saxy::csv::parser<saxy::second_throw_allocator> parser(i);
		INFO("Testing exception safety when buffer throws\n");
		INFO("Line: " << line << ", i = " << i);

		std::string::const_iterator save = csv.begin();
		try {
			const bool result = parser.parse(converter, csv.begin(), csv.end(), &save);
			CHECK(result);
			CHECK(save == csv.end());
		} catch(...) {
			CHECK(parser.parse(converter, save, csv.end(), &save));
			CHECK(save == csv.end());
		}

		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check exception safety when callback throw
	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		csv_test_parser converter(csv_test_parser::exception, static_cast<int>(i));
		saxy::csv::parser<> parser;
		INFO("Testing exception safety when callback throws\n");
		INFO("Line: " << line << ", i = " << i);

		std::string::const_iterator save = csv.begin();
		try {
			const bool result = parser.parse(converter, csv.begin(), csv.end(), &save);
			CHECK(result);
			CHECK(save == csv.end());
		} catch(...) {
			CHECK(parser.parse(converter, save, csv.end(), &save));
			CHECK(save == csv.end());
		}

		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check that stopping works
	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		csv_test_parser converter(csv_test_parser::stop, static_cast<int>(i));
		saxy::csv::parser<> parser;
		INFO("Testing stopping");
		INFO("Line: " << line << ", i = " << i);
		std::string::const_iterator save;
		CHECK(parser.parse(converter, csv.begin(), csv.end(), &save));
		CHECK(parser.parse(converter, save, csv.end(), &save));
		CHECK(save == csv.cend());
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}

	// Check that aborting works
	for(std::string::size_type i = 0; i < csv.size(); ++i) {
		csv_test_parser converter(csv_test_parser::abort, static_cast<int>(i));
		saxy::csv::parser<> parser;
		INFO("Testing aborting");
		INFO("Line: " << line << ", i = " << i);
		std::string::const_iterator save;
		if(!parser.parse(converter, csv.begin(), csv.end(), &save)) {
			CHECK(parser.parse(converter, save, csv.end(), &save));
		}

		CHECK(save == csv.cend());
		CHECK(converter.xml == xml);
		CHECK(converter.error_count == 0);
	}
}

TEST_CASE("Check conversion", "[csv]") {
	check_conversion(__LINE__, "A\r\n",                          "{[A]}");
	check_conversion(__LINE__, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n", "{[ABCDEFGHIJKLMNOPQRSTUVWXYZ]}");
	check_conversion(__LINE__, "A,B\r\n",                        "{[A][B]}");
	check_conversion(__LINE__, ",\r\n",                          "{[][]}");
	check_conversion(__LINE__, "AB,CD,EF\r\n",                   "{[AB][CD][EF]}");
	check_conversion(__LINE__, "A\r\nB\r\n",                     "{[A]}{[B]}");
	check_conversion(__LINE__, "A,B\r\nC,D\r\n",                 "{[A][B]}{[C][D]}");

	check_conversion(__LINE__, "\r\r\n",                         "{[\r]}");
	check_conversion(__LINE__, "A\r\r\n",                        "{[A\r]}");
	check_conversion(__LINE__, "\rA\r\n",                        "{[\rA]}");
	check_conversion(__LINE__, "A\rB\r\n",                       "{[A\rB]}");

	check_conversion(__LINE__, "\n\r\n",                         "{[\n]}");
	check_conversion(__LINE__, "A\n\r\n",                        "{[A\n]}");
	check_conversion(__LINE__, "\nA\r\n",                        "{[\nA]}");
	check_conversion(__LINE__, "A\nB\r\n",                       "{[A\nB]}");

	check_conversion(__LINE__, "\"\"\r\n",                       "{[]}");
	check_conversion(__LINE__, "\"\",\r\n",                      "{[][]}");
	check_conversion(__LINE__, "\"A\"\r\n",                      "{[A]}");
	check_conversion(__LINE__, "\"A,B\"\r\n",                    "{[A,B]}");
	check_conversion(__LINE__, "\"\n\"\r\n",                     "{[\n]}");
	check_conversion(__LINE__, "\"\r\"\r\n",                     "{[\r]}");
	check_conversion(__LINE__, "\"\r\n\"\r\n",                   "{[\r\n]}");
	check_conversion(__LINE__, "\"\"\"\"\r\n",                   "{[\"]}");
	check_conversion(__LINE__, "\"\"\"\"\"\"\r\n",               "{[\"\"]}");
	check_conversion(__LINE__, "\"\"\"A\"\"\",B\r\n",            "{[\"A\"][B]}");
}

TEST_CASE("Check equality", "[csv]") {
	struct {
		int line;
		int group;
		char const* str;
	} csv[] {
		// begin
		{ __LINE__,  -10, nullptr },

		// start_of_row
		{ __LINE__,  0, "A\r\n", },
		{ __LINE__,  0, "B,C\r\n", },

		// first_field_of_record
		// Possible to hit this if start_row throws

		// fail_on_line_feed
		{ __LINE__, 10, "\r", },

		// start_of_field
		{ __LINE__, 20, "B,", },
		{ __LINE__, 20, "\"B\",", },
		{ __LINE__, 20, ",", },

		// in_quoted_field
		{ __LINE__, 30, "\"", },
		{ __LINE__, 30, "A,\"", },
		{ __LINE__, 31, "\"B", },
		{ __LINE__, 31, "A,\"B", },
		{ __LINE__, 32, "\"C", },

		// in_unquoted_field
		{ __LINE__, 40, "A", },
		{ __LINE__, 40, "B,A", },
		{ __LINE__, 40, "B\r\nA", },
		{ __LINE__, 41, "B", },

		// in_quote
		{ __LINE__, 50, "\"A\"", },
		{ __LINE__, 50, "B,\"A\"", },
		{ __LINE__, 51, "\"B\"", },

		// in_new_line
		{ __LINE__, 60, ",\r", },
		{ __LINE__, 61, "A\r", },

		// require_line_feed
		{ __LINE__, 70, "\"\"\r", },
		{ __LINE__, 70, ",\"\"\r", },
		{ __LINE__, 71, "\"A\"\r", },

		// These 3 states are intermediate states
		// end_of_field
		// end_of_last_field
		// end_of_row

		// error
		{ __LINE__, 80, "A\"", },     // misplaced_double_quotes
		{ __LINE__, 80, "\"\"A", },   // text_after_closing_quotes
		{ __LINE__, 80, "\"\"\r,", }, // unfinished_crlf
	};

	std::size_t const count = sizeof(csv) / sizeof(csv[0]);
	for(std::size_t i = 0; i < count; ++i) {
		saxy::csv::parser<> lhs;
		csv_test_parser lhs_cb;
		if(csv[i].str) {
			lhs.parse(lhs_cb, csv[i].str, csv[i].str + std::strlen(csv[i].str));
		}
		for(std::size_t j = 0; j < count; ++j) {
			char buf[100];
			saxy::buffer buffer(buf, 100);
			saxy::stack_allocator<char> alloc(buffer);
			saxy::csv::parser<saxy::stack_allocator> rhs(100, alloc);
			csv_test_parser rhs_cb;
			if(csv[j].str) {
				rhs.parse(rhs_cb, csv[j].str, csv[j].str + std::strlen(csv[j].str));
			}

			if(csv[i].group == csv[j].group) {
				INFO("Checking equality: line " << csv[i].line << " vs " << csv[j].line);
				CHECK(lhs == rhs);
				CHECK(lhs.hash() == rhs.hash());
#ifdef SAXY_CPP11
				CHECK(std::hash<saxy::csv::parser<> >()(lhs) == std::hash<saxy::csv::parser<saxy::stack_allocator> >()(rhs));
#endif
			} else {
				INFO("Checking inequality: line " << csv[i].line << " vs " << csv[j].line);
				CHECK(lhs.hash() != rhs.hash());
				CHECK(lhs != rhs);
#ifdef SAXY_CPP11
				CHECK(std::hash<saxy::csv::parser<> >()(lhs) != std::hash<saxy::csv::parser<saxy::stack_allocator> >()(rhs));
#endif
			}
		}
	}
}

#if 0
TEST_CASE("CSV document finishes correctly", "[csv]") {
	{
		const std::string csv = "field";
		csv_test_parser converter;
		saxy::csv::parser parser;
		CHECK(parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.xml == "<r>");
		CHECK(converter.error_count == 0);

		parser.finish(converter);
		CHECK(converter.xml == "<r><c>field</c></r>");
	}

	{
		const std::string csv = "field\r";
		csv_test_parser converter;
		saxy::csv::parser parser;
		CHECK(parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.xml == "<r>");
		CHECK(converter.error_count == 0);

		parser.finish(converter);
		CHECK(converter.xml == "<r><c>field\r</c></r>");
	}

	{
		const std::string csv = "field\r\n";
		csv_test_parser converter;
		saxy::csv::parser parser;
		CHECK(parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.xml == "<r><c>field</c></r>");
		CHECK(converter.error_count == 0);

		parser.finish(converter);
		CHECK(converter.xml == "<r><c>field</c></r>");
	}

	{
		const std::string csv = "\"fie,ld";
		csv_test_parser converter;
		saxy::csv::parser parser;
		CHECK(parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.xml == "<r>");
		CHECK(converter.error_count == 0);

		parser.finish(converter);
		CHECK(converter.xml == "<r><c>fie,ld</c></r>");
	}
}
#endif

TEST_CASE("CSV errors are detected", "[csv]") {
	{
		const std::string csv = "misplaced \"quotes\"\r\n";
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		CHECK(!parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.error_count == 1);
		CHECK(converter.csv_error == saxy::csv::misplaced_double_quotes);
	}

	{
		const std::string csv = "\"field\" bad text";
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		CHECK(!parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.error_count == 1);
		CHECK(converter.csv_error == saxy::csv::text_after_closing_quotes);
	}

	{
		const std::string csv = "a,\"b\"\rc,d";
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		CHECK(!parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.error_count == 1);
		CHECK(converter.csv_error == saxy::csv::unfinished_crlf);
	}

	{
		const std::string csv = "";
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		CHECK(parser.parse(converter, csv.begin(), csv.end()));
		CHECK(!parser.finish(converter));
		CHECK(converter.error_count == 1);
		CHECK(converter.csv_error == saxy::csv::no_fields_in_record);
	}

	{
		const std::string csv = "a\r\n\r\n";
		csv_test_parser converter;
		saxy::csv::parser<> parser;
		CHECK(!parser.parse(converter, csv.begin(), csv.end()));
		CHECK(converter.error_count == 1);
		CHECK(converter.csv_error == saxy::csv::no_fields_in_record);
	}
}
