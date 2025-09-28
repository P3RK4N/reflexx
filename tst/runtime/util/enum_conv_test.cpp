#include "test_utils.hpp"

enum class Color { Red, Green, Blue };

TEST_CASE("enum_to_string constexpr valid values")
{
    STATIC_REQUIRE(enum_to_string(Color::Red  ) == "Red"  );
    STATIC_REQUIRE(enum_to_string(Color::Green) == "Green");
    STATIC_REQUIRE(enum_to_string(Color::Blue ) == "Blue" );
}

TEST_CASE("string_to_enum constexpr valid values")
{
    STATIC_REQUIRE(string_to_enum<Color>("Red"  ) == Color::Red  );
    STATIC_REQUIRE(string_to_enum<Color>("Green") == Color::Green);
    STATIC_REQUIRE(string_to_enum<Color>("Blue" ) == Color::Blue );
}

TEST_CASE("enum_to_string runtime valid values")
{
    REQUIRE(enum_to_string(Color::Red  ) == "Red"  );
    REQUIRE(enum_to_string(Color::Green) == "Green");
    REQUIRE(enum_to_string(Color::Blue ) == "Blue" );
}

TEST_CASE("string_to_enum runtime valid values")
{
    REQUIRE(string_to_enum<Color>("Red"  ) == Color::Red  );
    REQUIRE(string_to_enum<Color>("Green") == Color::Green);
    REQUIRE(string_to_enum<Color>("Blue" ) == Color::Blue );
}

TEST_CASE("enum_to_string asserts on invalid value")
{
    STATIC_REQUIRE      (is_compileable_v<[](){ enum_to_string(static_cast<Color>(1  )); }>);
    STATIC_REQUIRE_FALSE(is_compileable_v<[](){ enum_to_string(static_cast<Color>(123)); }>);
}

TEST_CASE("string_to_enum asserts on invalid name")
{
    STATIC_REQUIRE      (is_compileable_v<[](){ string_to_enum<Color>("Green"); }>);
    STATIC_REQUIRE_FALSE(is_compileable_v<[](){ string_to_enum<Color>("Pink" ); }>);
}
