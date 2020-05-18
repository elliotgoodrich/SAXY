#include "saxy/csv.hpp"
#include "saxy/stack_allocator.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

#include <cstdint>

//  Windows
#ifdef _MSC_VER

#include <intrin.h>
#pragma intrinsic(__rdtsc)
std::uint64_t rdtsc(){
    return __rdtsc();
}

//  Linux/GCC
#else

std::uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((std::uint64_t)hi << 32) | lo;
}

#endif

struct do_nothing {
	saxy::always_keep_going start_row() {
		return saxy::keep_going;
	}

	saxy::always_keep_going end_row() const {
		return saxy::keep_going;
	}

	saxy::always_keep_going field(saxy::string_cview) {
		return saxy::keep_going;
	}

	saxy::always_abort error(saxy::csv::error_code) {
		return saxy::abort;
	}
};

int main() {
	std::size_t const rows = 10000;
	std::size_t const columns = 10;

	std::string field = "0123456789012345678901234567890123456789012345678,\"901234567890123456\",78901234567890123456789012345678901234567890123456789";
	//std::string field = "0";
	std::string csv;

	{
		auto begin = std::chrono::high_resolution_clock::now();
		saxy::csv::generator gen(columns);
		auto out = std::back_inserter(csv);
		for(std::size_t row = 0; row < rows; ++row) {
			for(std::size_t column = 0; column < columns; ++column) {
				out = gen.add_field(out, field);
			}

			out = gen.finish_row(out);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto dur = end - begin;
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
		std::cout << "Generate time: " << us << "us\n";
	}

	std::size_t length;
	long long strlen_time;
	{
		auto begin = std::chrono::high_resolution_clock::now();
		auto tick_start = __rdtsc();
		length = std::strlen(csv.data());
		auto tick_end = __rdtsc();

		auto end = std::chrono::high_resolution_clock::now();
		auto dur = end - begin;
		auto tick_count = tick_end - tick_start;
		strlen_time = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
		strlen_time = std::max<long long>(strlen_time, 1);
		std::cout << length / 1024 << " Kb\n";
		std::cout << "std::strlen time: " << strlen_time << "us\n";
		std::cout << "MB/sec parsed: " << (1024.0 * 1024.0 * length) / (1000.0 * 1000.0 * strlen_time) << '\n';
		std::cout << "std::strlen ticks/char: " << static_cast<double>(tick_count) / length << "\n\n";
	}

#if 0
	{
		char buf[100];
		saxy::buffer buffer(buf, 100);
		saxy::stack_allocator<char> alloc(buffer);
		saxy::csv::parser<saxy::stack_allocator> parser(50, alloc);
		do_nothing cb;

		auto begin = std::chrono::high_resolution_clock::now();
		auto tick_start = __rdtsc();
		parser.parse(cb, csv.data(), csv.data() + csv.size());
		auto tick_end = __rdtsc();
		auto end = std::chrono::high_resolution_clock::now();

		auto dur = end - begin;
		auto tick_count = tick_end - tick_start;
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
		std::cout << "Parse time: " << us << "us\n";

		std::cout << "MB/sec parsed: " << (1024.0 * 1024.0 * length) / (1000.0 * 1000.0 * us) << '\n';
		std::cout << "ticks/char: " << static_cast<double>(tick_count) / length << "\n";
		std::cout << "strlen ratio: " << std::fixed << std::setprecision(2) << static_cast<double>(us) / strlen_time << "x\n\n";
	}
#endif

	double us = 0.0;
	double ticks = 0;
	const int how = 100;
	for (int i = 0; i < how; ++i) {
		do_nothing cb;

		auto begin = std::chrono::high_resolution_clock::now();
		auto tick_start = __rdtsc();
		saxy::csv::parse(cb, &csv[0], csv.size());
		auto tick_end = __rdtsc();
		auto end = std::chrono::high_resolution_clock::now();

		auto dur = end - begin;
		ticks += tick_end - tick_start;
		us += std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
	}

	us /= how;
	ticks /= how;
	std::cout << "In-place parse time: " << us << "us\n";
	std::cout << "MB/sec parsed: " << (1024.0 * 1024.0 * length) / (1000.0 * 1000.0 * us) << '\n';
	std::cout << "ticks/char: " << static_cast<double>(ticks / 10) / length << "\n";
	std::cout << "strlen ratio: " << std::fixed << std::setprecision(2) << static_cast<double>(us) / strlen_time << "x\n\n";
}
