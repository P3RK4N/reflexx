#include "test_utils.hpp"

TEST_CASE("serializable_number_v")
{
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<int32_t>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<uint64_t>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<float>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<double>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<unsigned char>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<signed char>);
    
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<long double>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<bool>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<char>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<char8_t>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<void>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<std::nullptr_t>);
}