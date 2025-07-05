#include <stdexcept>

#include <catch2/catch_all.hpp>

#include "reflexx/util/typename_chain.hpp"

using namespace reflexx::util;

TEST_CASE("typename_chain basic functionality", "[typename_chain]") {
    using chain0 = typename_chain<>;
    using chain3 = typename_chain<int, float, double>;

    SECTION("Size is correct") {
        STATIC_REQUIRE(chain0::size == 0);
        STATIC_REQUIRE(chain3::size == 3);
    }

    SECTION("At returns correct types") {
        STATIC_REQUIRE(std::is_same_v<chain3::at<0>, int>);
        STATIC_REQUIRE(std::is_same_v<chain3::at<1>, float>);
        STATIC_REQUIRE(std::is_same_v<chain3::at<2>, double>);
    }

    SECTION("Front and back") {
        STATIC_REQUIRE(std::is_same_v<chain3::front, int>);
        STATIC_REQUIRE(std::is_same_v<chain3::back, double>);
    }

    SECTION("Append adds types at the end") {
        using appended = chain3::append<char, double>;
        STATIC_REQUIRE(appended::size == 5);
        STATIC_REQUIRE(std::is_same_v<appended::at<3>, char>);
        STATIC_REQUIRE(std::is_same_v<appended::at<4>, double>);
    }

    SECTION("Ltrim removes first N types") {
        using trimmed = chain3::ltrim<1>;
        STATIC_REQUIRE(trimmed::size == 2);
        STATIC_REQUIRE(std::is_same_v<trimmed::at<0>, float>);
        STATIC_REQUIRE(std::is_same_v<trimmed::at<1>, double>);
    }

    SECTION("Slice returns range of types") {
        using sliced = chain3::slice<0, 2>;
        STATIC_REQUIRE(sliced::size == 2);
        STATIC_REQUIRE(std::is_same_v<sliced::at<0>, int>);
        STATIC_REQUIRE(std::is_same_v<sliced::at<1>, float>);
    }
}

TEST_CASE("typename_chain view produces correct string", "[typename_chain][view]") {
    using chain = typename_chain<int, float, double>;
    constexpr auto view_str = chain::view();
    STATIC_REQUIRE(view_str == "int.float.double");

    using empty_chain = typename_chain<>;
    constexpr auto empty_view = empty_chain::view();
    STATIC_REQUIRE(empty_view.empty());
}

TEST_CASE("typename_chain view_with_member appends member name", "[typename_chain][view_with_member]") {
    struct S { int member; };

    using chain = typename_chain<int, float>;
    constexpr auto full_view = chain::template view_with_member<^^S::member>();
    STATIC_REQUIRE(full_view == "int.float.member");

    using empty_chain = typename_chain<>;
    constexpr auto member_only_view = empty_chain::template view_with_member<^^S::member>();
    STATIC_REQUIRE(member_only_view == "member");
}
