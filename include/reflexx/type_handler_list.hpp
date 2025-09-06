#ifndef REFLEXX_TYPE_HANDLER_LIST_HPP
#define REFLEXX_TYPE_HANDLER_LIST_HPP

#include <cassert>
#include <experimental/meta>
#include <tuple>
#include <type_traits>
#include <utility>

#include "declare.hpp"

namespace reflexx {
namespace detail {

template <typename Handler, typename T, typename Sig, typename = void>
struct has_handle_sig : std::false_type {};

template <typename Handler, typename T, typename Sig>
struct has_handle_sig<Handler, T, Sig, std::void_t<decltype(static_cast<Sig>(&Handler::serialize))>> : std::true_type {};

template <typename Handler, typename T>
struct has_exact_handle_for : std::disjunction
<
    has_handle_sig<Handler, T, void (Handler::*)(T&)                           >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)                   noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&)                 &         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)                 & noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&)                &&         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)                && noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const                     >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const             noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const           &         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const           & noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const          &&         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const          && noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile            >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile    noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile  &         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile  & noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile &&         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&)       volatile && noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile            >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile    noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile  &         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile  & noexcept>,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile &&         >,
    has_handle_sig<Handler, T, void (Handler::*)(T&) const volatile && noexcept>
> {};

// Works for any callable, including base classes and convertible types
template <typename S, typename T>
concept HasCallableHandlerFor =
    requires    (S s, T& t) { s.serialize(t);                 } &&
    !requires   (S s      ) { s.serialize(std::declval<T>()); };

// Works only for exact matches, not for base classes or convertible types
template <typename Handler, typename T>
concept HasExactHandlerFor = detail::has_exact_handle_for<Handler, T>::value;

template <typename Handler, typename T, bool ExactOnly>
concept HasHandlerFor = HasExactHandlerFor<Handler, T> || (!ExactOnly && HasCallableHandlerFor<Handler, T>);

} // detail


template <template <typename, bool> class... Handlers>
struct type_handler_list final
{
private:
    template <IsTypeHandlerList TypeHandlerList>
    struct extend_impl;

    template <template <typename, bool> class... OtherHandlers>
    struct extend_impl<type_handler_list<OtherHandlers...>>
    {
        using type = type_handler_list<Handlers..., OtherHandlers...>;
    };

    template <typename TSerializer, bool IsReading, typename T, bool ExactOnly>
    static consteval std::size_t get_first_index()
    {
        std::size_t index = 0;

        // Go trough all Handlers
        template for (constexpr auto template_param : std::define_static_array(std::meta::template_arguments_of(^^type_handler_list)))
        {
            using THandler = [: template_param :]<TSerializer, IsReading>;

            if constexpr (detail::HasHandlerFor<THandler, T, ExactOnly>)
            {
                return index;
            }

            index++;
        }
        
        assert(false && "Could not find handler for type");
        std::unreachable();
    };

public:
    template <IsTypeHandlerList TypeHandlerList>
    using extend = extend_impl<TypeHandlerList>::type;

    template <template <typename, bool> class Handler>
    using append = type_handler_list<Handlers..., Handler>;

    template <typename TSerializer, bool IsReading>
    using handler_tuple_t = std::tuple<Handlers<TSerializer, IsReading>...>;

    template <typename TSerializer, bool IsReading, typename T, bool ExactOnly = true>
    static constexpr std::size_t get_first_index_v = get_first_index<TSerializer, IsReading, T, ExactOnly>();
};

} // reflexx

#endif