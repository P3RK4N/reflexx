#include <stdexcept>

#include <catch2/catch_all.hpp>

#include "reflexx/util/enum_conv.hpp"

using namespace reflexx::util;

enum class Color { Red, Green, Blue };
enum class Opaque : int;

TEST_CASE("enum_to_string constexpr valid values", "[enum][constexpr]") {
    STATIC_REQUIRE(enum_to_string(Color::Red) == "Red");
    STATIC_REQUIRE(enum_to_string(Color::Green) == "Green");
    STATIC_REQUIRE(enum_to_string(Color::Blue) == "Blue");
}

TEST_CASE("string_to_enum constexpr valid values", "[enum][constexpr]") {
    STATIC_REQUIRE(string_to_enum<Color>("Red") == Color::Red);
    STATIC_REQUIRE(string_to_enum<Color>("Green") == Color::Green);
    STATIC_REQUIRE(string_to_enum<Color>("Blue") == Color::Blue);
}

TEST_CASE("enum_to_string runtime valid values", "[enum][runtime]") {
    REQUIRE(enum_to_string(Color::Red) == "Red");
    REQUIRE(enum_to_string(Color::Green) == "Green");
    REQUIRE(enum_to_string(Color::Blue) == "Blue");
}

TEST_CASE("string_to_enum runtime valid values", "[enum][runtime]") {
    REQUIRE(string_to_enum<Color>("Red") == Color::Red);
    REQUIRE(string_to_enum<Color>("Green") == Color::Green);
    REQUIRE(string_to_enum<Color>("Blue") == Color::Blue);
}

TEST_CASE("enum_to_string throws on invalid value", "[enum][runtime][exception]") {
    REQUIRE_THROWS_AS(enum_to_string(static_cast<Color>(123)), std::invalid_argument);
}

TEST_CASE("string_to_enum throws on invalid name", "[enum][runtime][exception]") {
    REQUIRE_THROWS_AS(string_to_enum<Color>("Pink"), std::invalid_argument);
    REQUIRE_THROWS_WITH(string_to_enum<Color>("Pink"), Catch::Matchers::ContainsSubstring("Invalid enum name"));
}