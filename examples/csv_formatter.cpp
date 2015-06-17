#include "saxy/csv.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

struct csv_to_vector {
	std::vector<std::vector<std::string> > table;
	saxy::csv::error_code error_code;

	csv_to_vector()
	: error_code(saxy::csv::error_code::none) {
	}

	saxy::always_keep_going start_row() {
		table.push_back(std::vector<std::string>());
		return saxy::keep_going;
	}

	saxy::always_keep_going end_row() const {
		return saxy::keep_going;
	}

	saxy::always_keep_going field(saxy::string_cview str) {
		table.back().push_back(str.to_string());
		return saxy::keep_going;
	}

	saxy::always_abort error(saxy::csv::error_code code) {
		error_code = code;
		return saxy::abort;
	}
};

void print_hr(std::vector<std::size_t> const& column_width) {
	for(std::size_t col = 0; col < column_width.size(); ++col) {
		std::cout << '+';
		std::fill_n(std::ostream_iterator<char>(std::cout), column_width[col], '-');
	}
	std::cout << '+' << std::endl;
}

void print_row(std::vector<std::string> const& row,
               std::vector<std::size_t> const& column_width) {
	std::size_t col = 0;
	for(; col < row.size(); ++col) {
		std::cout << '|' << std::setw(column_width[col]) << row[col];
	}

	for(; col < column_width.size(); ++col) {
		std::cout << '|';
		std::fill_n(std::ostream_iterator<char>(std::cout), column_width[col], ' ');
	}

	std::cout << '|' << std::endl;
}

char const* error_code_to_string(saxy::csv::error_code ec) {
	switch(ec) {
		case saxy::csv::misplaced_double_quotes:
			return "Double quote found in unquoted field";
		case saxy::csv::text_after_closing_quotes:
			return "Text found after closing quote";
		case saxy::csv::unfinished_crlf:
			return "Line feed (\\n) expected here";
		default:
			return "Unknown error";
	}
}

int main() {
	std::cout << "Please enter a CSV file and enter a blank line to finish" << std::endl;

	csv_to_vector callback;
	saxy::csv::parser<> parser;

	// Read each line and parse the result
	std::string line;
	std::string const end_line = "\r\n";
	while(std::getline(std::cin, line) && !line.empty()) {
		std::string::const_iterator finish_it;
		if(!parser.parse(callback, line.cbegin(), line.cend(), &finish_it)) {
			std::cerr << "Parse error!\n";
			std::cerr << line << std::endl;
			const std::size_t error_index = finish_it - line.cbegin() - 1;
			std::fill_n(std::ostream_iterator<char>(std::cerr), error_index, ' ');
			std::cerr << "^----- " << error_code_to_string(callback.error_code) << std::endl;
			return 1;
		}

		parser.parse(callback, end_line.cbegin(), end_line.cend());
	}

	const std::vector<std::vector<std::string> >& table = callback.table;

	// Find the maximum size of each field in a column
	std::vector<std::size_t> column_width;
	for(std::size_t row = 0; row < table.size(); ++row) {
		// Extend column_width if necessary
		if(table[row].size() > column_width.size()) {
			const std::size_t excess_columns = table[row].size() - column_width.size();
			column_width.insert(column_width.end(), excess_columns, 0);
		}

		for(std::size_t col = 0; col < table[row].size(); ++col) {
			column_width[col] = std::max(column_width[col], table[row][col].size());
		}
	}

	// Print out the table
	if(!table.empty()) {
		print_hr(column_width);
		print_row(table[0], column_width);

		if(table.size() > 1) {
			print_hr(column_width);
		}

		for(std::size_t row = 1; row < table.size(); ++row) {
			print_row(table[row], column_width);
		}

		print_hr(column_width);
	}
}
