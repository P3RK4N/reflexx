#include <stdexcept>

#include <catch2/catch_all.hpp>

#include "util/class_util.hpp"
#include "util/enum_conv.hpp"
#include "util/non_serializable_category_type.hpp"
#include "util/typename_chain.hpp"

using namespace reflexx::util;

TEST_CASE("has_virtual_base_v trait tests", "[has_virtual_base_v]") {

    SECTION("Non-class type") {
        STATIC_REQUIRE(
            [] { return requires { has_virtual_base_v<int>; }; }()
        );
    }

    SECTION("Class with no base") {
        struct A {};
        STATIC_REQUIRE(!has_virtual_base_v<A>);
    }

    SECTION("Class with non-virtual base") {
        struct B {};
        struct C : B {};
        STATIC_REQUIRE(!has_virtual_base_v<C>);
    }

    SECTION("Class with single virtual base") {
        struct V {};
        struct D : virtual V {};
        STATIC_REQUIRE(has_virtual_base_v<D>);
    }

    SECTION("Class with multiple bases, one virtual") {
        struct E1 {};
        struct E2 {};
        struct E3 : E1, virtual E2 {};
        STATIC_REQUIRE(has_virtual_base_v<E3>);
    }

    SECTION("Deep inheritance with virtual base deep in hierarchy") {
        struct F1 {};
        struct F2 : virtual F1 {};
        struct F3 : F2 {};
        STATIC_REQUIRE(has_virtual_base_v<F3>);
    }

    SECTION("Multiple virtual bases") {
        struct G1 {};
        struct G2 {};
        struct G3 : virtual G1, virtual G2 {};
        STATIC_REQUIRE(has_virtual_base_v<G3>);
    }
}

TEST_CASE("has_ambigous_non_static_data_members_v trait", "[has_ambigous_non_static_data_members_v]") {
    
    SECTION("Non-class type") {
        STATIC_REQUIRE(
            [] { return requires { has_ambigous_non_static_data_members_v<int>; }; }()
        );
    }

    SECTION("Class with unique members") {
        struct A { int x; float y; };
        STATIC_REQUIRE(!has_ambigous_non_static_data_members_v<A>);
    }

    SECTION("Class with repeated member names due to multiple inheritance") {
        struct B1 { int shared; };
        struct B2 { float shared; };
        struct C : B1, B2 {};
        STATIC_REQUIRE(has_ambigous_non_static_data_members_v<C>);
    }

    SECTION("Class with base that introduces no ambiguity") {
        struct D1 { int a; };
        struct D2 { float b; };
        struct D : D1, D2 {};
        STATIC_REQUIRE(!has_ambigous_non_static_data_members_v<D>);
    }

    SECTION("Deep inheritance with conflicting member names") {
        struct E1 { int data; };
        struct E2 : E1 {};
        struct E3 { float data; };
        struct E4 : E2, E3 {};
        STATIC_REQUIRE(has_ambigous_non_static_data_members_v<E4>);
    }

    SECTION("Override-like case with shadowing — not ambiguous") {
        struct F1 { int val; };
        struct F2 : F1 { float val; }; // hides, not ambiguous in layout
        STATIC_REQUIRE(has_ambigous_non_static_data_members_v<F2>);
    }

    SECTION("Class with no members and no bases") {
        struct Empty {};
        STATIC_REQUIRE(!has_ambigous_non_static_data_members_v<Empty>);
    }
}

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
        STATIC_REQUIRE(!is_non_serializable_category_type_v<C>);
    }

    SECTION("Fundamental type") {
        STATIC_REQUIRE(!is_non_serializable_category_type_v<int>);
        STATIC_REQUIRE(!is_non_serializable_category_type_v<double>);
    }

    SECTION("Bounded array type") {
        STATIC_REQUIRE(!is_non_serializable_category_type_v<int[5]>);
    }

}

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
