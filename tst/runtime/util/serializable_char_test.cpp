#include "test_utils.hpp"

TEST_CASE("serializable_char_v")
{
    STATIC_REQUIRE      (is_serializable_char_v<char>           );
    STATIC_REQUIRE      (is_serializable_char_v<char8_t>        );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<signed char>    );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<unsigned char>  );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<wchar_t>        );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<char16_t>       );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<char32_t>       );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<int>            );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<float>          );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<double>         );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<long double>    );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<bool>           );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<void>           );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<std::nullptr_t> );
}