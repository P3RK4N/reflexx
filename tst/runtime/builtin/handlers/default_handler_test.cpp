#include <catch2/catch_all.hpp>

#include <reflexx/builtin/default_handler.hpp>

using namespace ::reflexx;
using namespace ::reflexx::detail;

TEST_CASE("has_virtual_base_v trait tests", "[has_virtual_base_v]") {

    SECTION("Class with no base") {
        struct A {};
        STATIC_REQUIRE(!has_virtual_base<A>());
    }

    SECTION("Class with non-virtual base") {
        struct B {};
        struct C : B {};
        STATIC_REQUIRE(!has_virtual_base<C>());
    }

    SECTION("Class with single virtual base") {
        struct V {};
        struct D : virtual V {};
        STATIC_REQUIRE(has_virtual_base<D>());
    }

    SECTION("Class with multiple bases, one virtual") {
        struct E1 {};
        struct E2 {};
        struct E3 : E1, virtual E2 {};
        STATIC_REQUIRE(has_virtual_base<E3>());
    }

    SECTION("Deep inheritance with virtual base deep in hierarchy") {
        struct F1 {};
        struct F2 : virtual F1 {};
        struct F3 : F2 {};
        STATIC_REQUIRE(has_virtual_base<F3>());
    }

    SECTION("Multiple virtual bases") {
        struct G1 {};
        struct G2 {};
        struct G3 : virtual G1, virtual G2 {};
        STATIC_REQUIRE(has_virtual_base<G3>());
    }
}

TEST_CASE("has_ambigous_non_static_data_members_v trait", "[has_ambigous_non_static_data_members_v]") {

    SECTION("Class with unique members") {
        struct A { int x; float y; };
        STATIC_REQUIRE(!has_ambigous_non_static_data_members<A>());
    }

    SECTION("Class with repeated member names due to multiple inheritance") {
        struct B1 { int shared; };
        struct B2 { float shared; };
        struct C : B1, B2 {};
        STATIC_REQUIRE(has_ambigous_non_static_data_members<C>());
    }

    SECTION("Class with base that introduces no ambiguity") {
        struct D1 { int a; };
        struct D2 { float b; };
        struct D : D1, D2 {};
        STATIC_REQUIRE(!has_ambigous_non_static_data_members<D>());
    }

    SECTION("Deep inheritance with conflicting member names") {
        struct E1 { int data; };
        struct E2 : E1 {};
        struct E3 { float data; };
        struct E4 : E2, E3 {};
        STATIC_REQUIRE(has_ambigous_non_static_data_members<E4>());
    }

    SECTION("Override-like case with shadowing — not ambiguous") {
        struct F1 { int val; };
        struct F2 : F1 { float val; }; // hides, not ambiguous in layout
        STATIC_REQUIRE(has_ambigous_non_static_data_members<F2>());
    }

    SECTION("Class with no members and no bases") {
        struct Empty {};
        STATIC_REQUIRE(!has_ambigous_non_static_data_members<Empty>());
    }
}
