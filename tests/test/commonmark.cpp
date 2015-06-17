#include "catch/catch.hpp"

#include "saxy/commonmark.hpp"

#include <string>
#include <vector>

struct commonmark_to_html {
	std::string html;

	saxy::always_keep_going horizontal_line() {
		html += "<hr />";
		return saxy::keep_going;
	}

	saxy::always_keep_going start_paragraph() {
		html += "<p>";
		return saxy::keep_going;
	}

	saxy::always_keep_going text(std::string const& str) {
		html += str;
		return saxy::keep_going;
	}

	saxy::always_keep_going end_paragraph() {
		html += "</p>";
		return saxy::keep_going;
	}
};

std::string to_html(std::string const& commonmark) {
	commonmark_to_html converter;
	saxy::commonmark_parser parser;
	parser.parse(converter, commonmark.begin(), commonmark.end());
	return converter.html;
}

TEST_CASE("4.1 Horizontal rules", "[commonmark][leaf block]") {
	CHECK(to_html("***") == "<hr />");
}

