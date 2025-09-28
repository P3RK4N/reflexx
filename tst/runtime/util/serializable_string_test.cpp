#include "test_utils.hpp"

TEST_CASE("serializable_string_v")
{
    STATIC_REQUIRE      (is_serializable_string_v<std::string>                       );
    STATIC_REQUIRE      (is_serializable_string_v<std::u8string>                     );
    STATIC_REQUIRE      (is_serializable_string_v<std::string_view>                  );
    STATIC_REQUIRE      (is_serializable_string_v<std::u8string_view>                );

    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::wstring>       );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u16string>     );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u32string>     );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::wstring_view>  );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u16string_view>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u32string_view>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<char>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<int>                );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<float>              );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<double>             );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<long double>        );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<bool>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<void>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::nullptr_t>     );
}
