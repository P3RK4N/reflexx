#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "reflexx/util/function_traits.hpp"

using namespace reflexx::util;

int free_func          (int, double)          { return 42; }
int free_func_noexcept (int        ) noexcept { return  0; }

struct Foo
{
    int f                (int)                            { return 1; }
    int f_const          (int) const                      { return 2; }
    int f_volatile       (int)       volatile             { return 3; }
    int f_cv             (int) const volatile             { return 4; }
    int f_lref           (int)                 &          { return 5; }
    int f_rref           (int)                &&          { return 6; }
    int f_noexcept       (int)                   noexcept { return 7; }
    int f_const_noexcept (int) const             noexcept { return 8; }
    int f_everyone       (int) const volatile && noexcept { return 9; }
};

struct Functor
{
    int operator() (int, double)       { return 10; }
    int operator() (int        ) const { return 11; }
};

TEST_CASE("function_traits on free functions")
{
    using traits = function_traits<decltype(free_func)>;
    STATIC_REQUIRE(std::is_same_v<typename traits::return_type, int>);
    STATIC_REQUIRE(traits::args_size == 2);
    STATIC_REQUIRE(std::is_same_v<typename traits::args_types, std::tuple<int, double>>);

    using traits_noexcept = function_traits<decltype(free_func_noexcept)>;
    STATIC_REQUIRE(std::is_same_v<typename traits_noexcept::return_type, int>);
    STATIC_REQUIRE(traits_noexcept::args_size == 1);
    STATIC_REQUIRE(std::is_same_v<typename traits_noexcept::args_types, std::tuple<int>>);
}

TEST_CASE("function_traits on function pointers")
{
    using traits = function_traits<int(*)(int, double)>;
    STATIC_REQUIRE(std::is_same_v<typename traits::return_type, int>);
    STATIC_REQUIRE(traits::args_size == 2);
}

TEST_CASE("function_traits on function references are well defined")
{
    using traits = function_traits<int(&)(int, double)>;
    STATIC_REQUIRE(std::is_same_v<typename traits::return_type, int>);
    STATIC_REQUIRE(traits::args_size == 2);
}

TEST_CASE("function_traits on various member functions with qualifiers are well defined")
{
    using traits1 = function_traits<decltype(&Foo::f)>;
    using traits2 = function_traits<decltype(&Foo::f_const)>;
    using traits3 = function_traits<decltype(&Foo::f_volatile)>;
    using traits4 = function_traits<decltype(&Foo::f_cv)>;
    using traits5 = function_traits<decltype(&Foo::f_lref)>;
    using traits6 = function_traits<decltype(&Foo::f_rref)>;
    using traits7 = function_traits<decltype(&Foo::f_noexcept)>;
    using traits8 = function_traits<decltype(&Foo::f_const_noexcept)>;
    using traits9 = function_traits<decltype(&Foo::f_everyone)>;
}

TEST_CASE("function_traits on lambdas are well defined")
{
    auto lam            = [     ] (int, double)                         { return 11;     };
    auto lam_const      = [     ] (int        )                  -> int { return 12;     };
    auto lam_mut        = [x = 0] (           ) mutable                 { return x + 13; };
    auto lam_noexcept   = [     ] (int        )         noexcept        { return 14;     };

    using traits            = function_traits<decltype(lam         )>;
    using traits_const      = function_traits<decltype(lam_const   )>;
    using traits_mut        = function_traits<decltype(lam_mut     )>;
    using traits_noexcept   = function_traits<decltype(lam_noexcept)>;
}

TEST_CASE("function_traits on functor with overloaded operator()")
{
    using traits1 = function_traits<decltype(static_cast<int(Functor::*)(int, double)>(&Functor::operator()))>;
    STATIC_REQUIRE(std::is_same_v<typename traits1::return_type, int>);
    STATIC_REQUIRE(traits1::args_size == 2);

    using traits2 = function_traits<decltype(static_cast<int(Functor::*)(int) const>(&Functor::operator()))>;
    STATIC_REQUIRE(std::is_same_v<typename traits2::return_type, int>);
    STATIC_REQUIRE(traits2::args_size == 1);
}

TEST_CASE("static_foreach_arg runs over all args properly")
{
    constexpr int count = []() constexpr
    {
        int int_count = 0;

        function_traits<void(int, double, char, int)>::static_foreach_arg([&](auto info) consteval
        {
            int_count += info == ^^int;
        });

        return int_count;
    }();
    
    STATIC_REQUIRE(count == 2);
}


TEST_CASE("static_all_of_args works correctly")
{
    constexpr bool all_int = []()
    {
        return function_traits<void(int, int, int)>::static_all_of_args([](auto info){ return info == ^^int; });
    }();
    
    constexpr bool not_all_int = []()
    {
        return function_traits<void(int, double, int)>::static_all_of_args([](auto info){ return info == ^^int; });
    }();
    
    constexpr bool empty_all = []()
    {
        return function_traits<int()>::static_all_of_args([](auto info){ return false;});
    }();
    
    STATIC_REQUIRE(all_int == true);
    STATIC_REQUIRE(not_all_int == false);
    STATIC_REQUIRE(empty_all == true);
}

TEST_CASE("static_any_of_args works correctly")
{
    constexpr bool any_double = []()
    {
        return function_traits<void(int, double, char)>::static_any_of_args([](auto info){ return info == ^^double; });
    }();

    constexpr bool none_float = []()
    {
        return function_traits<void(int, double, char)>::static_any_of_args([](auto info){ return info == ^^float; });
    }();

    constexpr bool empty_any = []()
    {
        return function_traits<int()>::static_any_of_args([](auto info){ return true; });
    }();

    STATIC_REQUIRE(any_double == true);
    STATIC_REQUIRE(none_float == false);
    STATIC_REQUIRE(empty_any == false);
}

TEST_CASE("function_traits equality")
{
    using traits_plain        = function_traits<void(int&, double)>;
    using traits_arg_volatile = function_traits<void(volatile int&, double)>;
    using traits_arg_cv       = function_traits<void(const int, volatile double&)>;
    using traits_same         = function_traits<void(int&, double)>;

    STATIC_REQUIRE(are_function_traits_same_v<traits_plain, traits_same>);

    STATIC_REQUIRE(!are_function_traits_same_v<traits_plain, traits_arg_volatile>);
    STATIC_REQUIRE(!are_function_traits_same_v<traits_plain, traits_arg_cv>);
}