#include "test_utils.hpp"

TEST_CASE("custom_serializable_class_v")
{
    STATIC_REQUIRE      (is_serializable_class_v<TestStruct>        );

    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::string>       );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::u8string>     );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::string_view>  );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::u8string_view>);
    STATIC_REQUIRE_FALSE(is_serializable_class_v<TestEnum>          );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<int>               );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<float>             );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<double>            );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<bool>              );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<void>              );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::nullptr_t>    );
}
