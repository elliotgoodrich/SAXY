#include "catch/catch.hpp"

#include "saxy/string_view.hpp"

TEST_CASE("string_view comparison", "[string_view]") {
	int const count = 9;
	char const* string[count] = {
		"",
		"0",
		"1",
		"123",
		"132",
		"18",
		"19",
		"3210",
		"9",
	};

	for(int i = 0; i < count; ++i) {
		INFO("LHS Index: " << i);
		char const* lhs = string[i];

		// Equality
		CHECK(saxy::string_cview(lhs) == saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) == lhs);
		CHECK(lhs == saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) == std::string(lhs));
		CHECK(std::string(lhs) == saxy::string_cview(lhs));

		// Inequality
		CHECK(!(saxy::string_cview(lhs) != saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) != lhs));
		CHECK(!(lhs != saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) != std::string(lhs)));
		CHECK(!(std::string(lhs) != saxy::string_cview(lhs)));

		// Less than
		CHECK(!(saxy::string_cview(lhs) < saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) < lhs));
		CHECK(!(lhs < saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) < std::string(lhs)));
		CHECK(!(std::string(lhs) < saxy::string_cview(lhs)));

		// Less than or equal
		CHECK(saxy::string_cview(lhs) <= saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) <= lhs);
		CHECK(lhs <= saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) <= std::string(lhs));
		CHECK(std::string(lhs) <= saxy::string_cview(lhs));

		// Greater than
		CHECK(!(saxy::string_cview(lhs) > saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) > lhs));
		CHECK(!(lhs > saxy::string_cview(lhs)));
		CHECK(!(saxy::string_cview(lhs) > std::string(lhs)));
		CHECK(!(std::string(lhs) > saxy::string_cview(lhs)));

		// Greater than or equal
		CHECK(saxy::string_cview(lhs) >= saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) >= lhs);
		CHECK(lhs >= saxy::string_cview(lhs));
		CHECK(saxy::string_cview(lhs) >= std::string(lhs));
		CHECK(std::string(lhs) >= saxy::string_cview(lhs));

		for(int j = i + 1; j < count; ++j) {
			INFO("RHS Index: " << j);
			char const* rhs = string[j];

			// Equality
			CHECK(!(saxy::string_cview(lhs) == saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) == rhs));
			CHECK(!(lhs == saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) == std::string(rhs)));
			CHECK(!(std::string(lhs) == saxy::string_cview(rhs)));

			// Inequality
			CHECK(saxy::string_cview(lhs) != saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) != rhs);
			CHECK(lhs != saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) != std::string(rhs));
			CHECK(std::string(lhs) != saxy::string_cview(rhs));

			// Less than
			CHECK(saxy::string_cview(lhs) < saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) < rhs);
			CHECK(lhs < saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) < std::string(rhs));
			CHECK(std::string(lhs) < saxy::string_cview(rhs));

			// Less than or equal
			CHECK(saxy::string_cview(lhs) <= saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) <= rhs);
			CHECK(lhs <= saxy::string_cview(rhs));
			CHECK(saxy::string_cview(lhs) <= std::string(rhs));
			CHECK(std::string(lhs) <= saxy::string_cview(rhs));

			// Greater than
			CHECK(!(saxy::string_cview(lhs) > saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) > rhs));
			CHECK(!(lhs > saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) > std::string(rhs)));
			CHECK(!(std::string(lhs) > saxy::string_cview(rhs)));

			// Greater than or equal
			CHECK(!(saxy::string_cview(lhs) >= saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) >= rhs));
			CHECK(!(lhs >= saxy::string_cview(rhs)));
			CHECK(!(saxy::string_cview(lhs) >= std::string(rhs)));
			CHECK(!(std::string(lhs) >= saxy::string_cview(rhs)));
		}
	}
}

TEST_CASE("string_view constructors", "[string_view]") {
	CHECK(saxy::string_view() == "");
	CHECK(saxy::string_cview() == "");

	char text[] = "text";
	CHECK(saxy::string_view(text) == "text");
	CHECK(saxy::string_cview(text) == "text");
	CHECK(saxy::string_cview("text") == "text");

	char const* ptext = "text";
	CHECK(saxy::string_view(text + 2) == "xt");
	CHECK(saxy::string_cview(text + 2) == "xt");
	CHECK(saxy::string_cview(ptext + 2) == "xt");

	CHECK(saxy::string_view(text, 3) == "tex");
	CHECK(saxy::string_cview(text, 3) == "tex");
	CHECK(saxy::string_cview(ptext, 3) == "tex");

	std::string str = "text";
	const std::string cstr = "text";
	CHECK(saxy::string_view(str) == "text");
	CHECK(saxy::string_cview(str) == "text");
	CHECK(saxy::string_cview(cstr) == "text");

	saxy::string_cview copy = "text";
	//CHECK(saxy::string_view(copy) == "text");
	CHECK(saxy::string_cview(copy) == "text");
}

TEST_CASE("string_cview::empty", "[string_cview]") {
	CHECK(saxy::string_cview().empty());
	CHECK(saxy::string_cview("").empty());
	CHECK(!saxy::string_cview("a").empty());
}

TEST_CASE("string_cview::size", "[string_cview]") {
	CHECK(saxy::string_cview().size() == 0u);
	CHECK(saxy::string_cview("").size() == 0u);
	CHECK(saxy::string_cview("a").size() == 1u);
	CHECK(saxy::string_cview("hello world").size() == 11u);
}

TEST_CASE("string_cview accessors", "[string_cview]") {
	saxy::string_cview str("hello");
	CHECK(str.front() == 'h');
	CHECK(str[0] == 'h');
	CHECK(str[1] == 'e');
	CHECK(str[2] == 'l');
	CHECK(str[3] == 'l');
	CHECK(str[4] == 'o');
	CHECK(str.back() == 'o');

	char copy[] = "hello";
	CHECK(std::equal(str.begin(), str.end(), copy));
	CHECK(std::equal(str.cbegin(), str.cend(), copy));

	saxy::string_cview cstr("hello");
	CHECK(cstr.front() == 'h');
	CHECK(cstr[0] == 'h');
	CHECK(cstr[1] == 'e');
	CHECK(cstr[2] == 'l');
	CHECK(cstr[3] == 'l');
	CHECK(cstr[4] == 'o');
	CHECK(cstr.back() == 'o');
}

TEST_CASE("string_view modifiers", "[string_view]") {
	char data[] = "hello";
	saxy::string_view str(data);
	CHECK(str == "hello");

	str.front() = 'k';
	CHECK(str == "kello");

	str[2] = 'i';
	CHECK(str == "keilo");

	str.back() = 'p';
	CHECK(str == "keilp");

	*(str.begin() + 1) = 'r';
	CHECK(str == "krilp");
}

TEST_CASE("string_cview::clear", "[string_cview]") {
	saxy::string_cview str("abcdef");
	CHECK(str == "abcdef");
	str.clear();
	CHECK(str.empty());
	CHECK(str == "");
}

TEST_CASE("string_cview::swap", "[string_cview]") {
	{
		saxy::string_cview left = "left";
		saxy::string_cview right = "right";
		left.swap(right);
		CHECK(left == "right");
		CHECK(right == "left");

		right.swap(left);
		CHECK(left == "left");
		CHECK(right == "right");
	}

	{
		saxy::string_cview left = "left";
		saxy::string_cview right = "right";
		left.swap(right);
		CHECK(left == "right");
		CHECK(right == "left");

		right.swap(left);
		CHECK(left == "left");
		CHECK(right == "right");
	}
}

TEST_CASE("string_cview out stream", "[string_cview]") {
	std::stringstream ss;
	ss << saxy::string_cview("out");
	CHECK(ss.str() == "out");
	ss.str("");
	ss << saxy::string_cview("out");
	CHECK(ss.str() == "out");
}
