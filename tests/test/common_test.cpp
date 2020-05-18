#include <bitset>

#include "catch/catch.hpp"

#include "saxy/common.hpp"

void check(unsigned data, int expected) {
	CHECK(saxy::detail::count_leading_zeros(data) == expected);
}

TEST_CASE("Count leading zeros", "[count_leading_zeros]") {
	check(0, 16);
	for (int i = 1; i < 16; ++i) {
		check(1u << i, i);
	}
	check(32768, 15);
	check(49152, 14);
}
