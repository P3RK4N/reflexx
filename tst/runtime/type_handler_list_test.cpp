#include "reflexx/declare.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstddef>

#include <reflexx/type_handler_list.hpp>

/*
    ########################################
    #            HasHandler tests          #
    ########################################
*/

struct Base           {};
struct Derived : Base {};

struct BaseHandler              {                         void serialize(Base&)           {} };
struct DerivedHandler           {                         void serialize(Derived&)        {} };
struct ConstDerivedHandler      {                         void serialize(const Derived&)  {} };
struct TemplatedBaseHandler     { template <bool = false> void serialize(Base&)           {} };
struct TemplatedDerivedHandler  { template <bool = false> void serialize(Derived&)        {} };
struct WildcardHandler          { template <typename T  > void serialize(T&)              {} };

TEST_CASE("[Exact] HasHandlerFor detects valid overloads", "[HasHandlerFor]")
{
    STATIC_REQUIRE_FALSE(reflexx::detail::HasHandlerFor<            BaseHandler, Derived, true>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<         DerivedHandler, Derived, true>);
    STATIC_REQUIRE_FALSE(reflexx::detail::HasHandlerFor<    ConstDerivedHandler, Derived, true>);
    STATIC_REQUIRE_FALSE(reflexx::detail::HasHandlerFor<   TemplatedBaseHandler, Derived, true>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<TemplatedDerivedHandler, Derived, true>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<        WildcardHandler, Derived, true>);
}

TEST_CASE("[Callable] HasHandlerFor detects valid overloads", "[HasHandlerFor]")
{
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<            BaseHandler, Derived, false>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<         DerivedHandler, Derived, false>);
    STATIC_REQUIRE_FALSE(reflexx::detail::HasHandlerFor<    ConstDerivedHandler, Derived, false>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<   TemplatedBaseHandler, Derived, false>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<TemplatedDerivedHandler, Derived, false>);
    STATIC_REQUIRE      (reflexx::detail::HasHandlerFor<        WildcardHandler, Derived, false>);
}

/*
    ########################################
    #         type_handler_list tests      #
    ########################################
*/

// Dummy types
struct A {};
struct B {};
struct C {};
struct D : public A {};

struct DummySerializer {};

template <typename T, bool R>         struct handler_A { void serialize(A&) {}               };
template <typename T, bool R = false> struct handler_B { void serialize(B&) requires (!R) {} };

template <typename S, bool R>
struct fallback_handler { void serialize(auto&) {} };

TEST_CASE("type_handler_list resolves first matching handler", "[get_first_t]")
{
    using List = reflexx::type_handler_list<handler_B, handler_A, fallback_handler>;

    STATIC_REQUIRE(List::get_first_index_v<DummySerializer, false, B> == 0);
    STATIC_REQUIRE(List::get_first_index_v<DummySerializer, true, A> == 1);
    STATIC_REQUIRE(List::get_first_index_v<DummySerializer, true, D> == 2);
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