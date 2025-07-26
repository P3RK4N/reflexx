#include <stdexcept>

#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "reflexx/util/non_serializable_category_type.hpp"

using namespace reflexx::util;

TEST_CASE("is_non_serializable_category_type_v trait", "[type_category]") {

    SECTION("Lvalue reference") {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int&>);
    }

    SECTION("Rvalue reference") {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int&&>);
    }

    SECTION("Pointer to int") {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int*>);
    }

    SECTION("Pointer to member") {
        struct S { int m; };
        STATIC_REQUIRE(is_non_serializable_category_type_v<int S::*>);
    }

    SECTION("Unbounded array") {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int[]>);
    }

    SECTION("Function type") {
        using Fn = void(int);
        STATIC_REQUIRE(is_non_serializable_category_type_v<Fn>);
    }

    SECTION("Union type") {
        union U { int a; float b; };
        STATIC_REQUIRE(is_non_serializable_category_type_v<U>);
    }

    SECTION("Class type") {
        struct C {};
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<C>);
    }

    SECTION("Fundamental type") {
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<int>);
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<double>);
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<bool>);
    }

    SECTION("Bounded array type") {
        STATIC_REQUIRE(!is_non_serializable_category_type_v<int[5]>);
    }

}
