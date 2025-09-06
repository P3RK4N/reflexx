#ifndef REFLEXX_FUNCTION_TRAITS
#define REFLEXX_FUNCTION_TRAITS

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <meta>

#include "reflexx/util/unconstructible.hpp"

namespace reflexx::util {
namespace detail {

/* 
    ############################################
    ################ FALLBACK ##################
    ############################################
*/

template<typename T, typename = void>
struct function_traits : unconstructible
{
    static_assert(false, "Unsupported object!");
};

/* 
    ############################################
    ############## FREE FUNCTION ###############
    ############################################
*/

template <typename R, typename... Args>
struct function_traits<R(Args...)> : unconstructible
{
    using return_type = R;
    using args_types = std::tuple<Args...>;
    inline static constexpr std::size_t args_size = sizeof...(Args);

    template <typename Callable>
    requires std::is_invocable_v<Callable, std::meta::info>
    inline static consteval void static_foreach_arg(Callable c)
    {
        (c(^^Args), ...);
    }

    template <typename Callable>
    requires std::is_invocable_r_v<bool, Callable, std::meta::info>
    inline static consteval bool static_all_of_args(Callable c)
    {
        return (c(^^Args) && ... && true);
    }
    
    template <typename Callable>
    requires std::is_invocable_r_v<bool, Callable, std::meta::info>
    inline static consteval bool static_any_of_args(Callable c)
    {
        return (c(^^Args) || ... || false);
    }
};

template <typename R, typename... Args>
struct function_traits<R(Args...)          noexcept> : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R(Args...) volatile         > : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R(Args...) volatile noexcept> : function_traits<R(Args...)> {};

/* 
    ############################################
    ########### FREE FUNCTION PTR ##############
    ############################################
*/

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)         > : function_traits<R(Args...)> {};

template<typename R, typename... Args>
struct function_traits<R(*)(Args...) noexcept> : function_traits<R(Args...)> {};

/* 
    ############################################
    ########### FREE FUNCTION REF ##############
    ############################################
*/

template <typename R, typename... Args>
struct function_traits<R(&)(Args...)         > : function_traits<R(Args...)> {};

template <typename R, typename... Args>
struct function_traits<R(&)(Args...) noexcept> : function_traits<R(Args...)> {};

/* 
    ############################################
    ############ MEMBER FUNCTION ###############
    ############################################
*/

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                           > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const                     > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile            > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile            > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                 &         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const           &         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile  &         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile  &         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                &&         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const          &&         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile &&         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile &&         > : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                   noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const             noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile    noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile    noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                 & noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const           & noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile  & noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile  & noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)                && noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const          && noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)       volatile && noexcept> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const volatile && noexcept> : function_traits<R(Args...)> {};

/* 
    ############################################
    ############## OPERATOR() ##################
    ############################################
*/

template <typename T>
struct function_traits<T, std::void_t<decltype(&T::operator())>> : function_traits<decltype(&T::operator())> {};

} // detail
    
template <typename T>
struct function_traits : detail::function_traits<T> {};

template <typename T>
struct is_function_traits_t                     : std::false_type {};

template <typename T>
struct is_function_traits_t<function_traits<T>> : std::true_type  {};

template <typename T>
concept is_function_traits = is_function_traits_t<T>::value;

template <typename T1, typename T2>
inline constexpr bool are_function_traits_same_v = 
    std::is_same_v<typename T1::return_type, typename T2::return_type> && 
    std::is_same_v<typename T1::args_types,  typename T2::args_types >;

} // reflexx::util

#endif