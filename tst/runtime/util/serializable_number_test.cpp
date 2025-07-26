#include <catch2/catch_all.hpp>

#include <reflexx/util/serializable_number.hpp>

TEST_CASE("serializable_number_v") {
    REQUIRE(reflexx::util::is_serializable_number_v<int32_t>);
    REQUIRE(reflexx::util::is_serializable_number_v<uint64_t>);
    REQUIRE(reflexx::util::is_serializable_number_v<float>);
    REQUIRE(reflexx::util::is_serializable_number_v<double>);

    REQUIRE_FALSE(reflexx::util::is_serializable_number_v<bool>);
    REQUIRE_FALSE(reflexx::util::is_serializable_number_v<char>);
    REQUIRE_FALSE(reflexx::util::is_serializable_number_v<void>);
    REQUIRE_FALSE(reflexx::util::is_serializable_number_v<std::nullptr_t>);
    REQUIRE_FALSE(reflexx::util::is_serializable_number_v<long double>);
}