#include "test_utils.hpp"

enum class MyEnum    { A, B, C };
enum class EmptyEnum {         };
enum       OldEnum   { X, Y, Z };

struct NotEnum {};

TEST_CASE("serializable_enum_v")
{
    STATIC_REQUIRE(is_serializable_enum_v<MyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<OldEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<EmptyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<const MyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<volatile OldEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<const volatile MyEnum>);

    STATIC_REQUIRE_FALSE(is_serializable_enum_v<int>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<float>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<double>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<char>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<bool>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<void>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<std::nullptr_t>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<NotEnum>);
}