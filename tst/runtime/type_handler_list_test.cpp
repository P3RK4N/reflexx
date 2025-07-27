#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include <concepts>
#include <utility>

#include <reflexx/type_handler_list.hpp>

// Dummy types
struct A {};
struct B {};
struct C {};

struct DummySerializer {};

// Handler that matches A
template <typename S, bool R>
struct handler_A
{
    void serialize(A&) {}
};

// Handler that matches B (read-only only)
template <typename S, bool R>
struct handler_B
{
    void serialize(B&) requires (!R) {}
};

// Fallback handler (would match anything)
template <typename S, bool R>
struct fallback_handler
{
    void serialize(auto&) {}
};

TEST_CASE("HasHandlerFor detects valid overloads", "[HasHandlerFor]")
{
    STATIC_REQUIRE(reflexx::HasHandlerFor<handler_A<DummySerializer, true>, A>);
    STATIC_REQUIRE_FALSE(reflexx::HasHandlerFor<handler_A<DummySerializer, true>, B>);

    STATIC_REQUIRE(reflexx::HasHandlerFor<handler_B<DummySerializer, false>, B>);
    STATIC_REQUIRE_FALSE(reflexx::HasHandlerFor<handler_B<DummySerializer, true>, B>);
}

TEST_CASE("type_handler_list resolves first matching handler", "[get_first_t]")
{
    using List = reflexx::type_handler_list<handler_B, handler_A>;

    STATIC_REQUIRE(List::get_first_index_v<DummySerializer, false, B> == 0);
    STATIC_REQUIRE(List::get_first_index_v<DummySerializer, true, A> == 1);
}

TEST_CASE("append correctly adds new handler", "[append]")
{
    using L1 = reflexx::type_handler_list<handler_A>;
    using L2 = L1::append<handler_B>;

    STATIC_REQUIRE(L2::get_first_index_v<DummySerializer, false, B> == 1);
}

TEST_CASE("extend merges handler lists", "[extend]")
{
    using L1 = reflexx::type_handler_list<handler_A>;
    using L2 = reflexx::type_handler_list<handler_B>;
    using L3 = L1::extend<L2>;

    STATIC_REQUIRE(L3::get_first_index_v<DummySerializer, false, B> == 1);
}

