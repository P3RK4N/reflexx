#ifndef REFLEXX_HANDLER_LIST_HPP
#define REFLEXX_HANDLER_LIST_HPP

#include <cassert>
#include <concepts>
#include <experimental/meta>

#include "reflexx/custom_type_handler.hpp"

namespace reflexx {

template <typename S, typename T>
concept HasHandlerFor =
    requires    (S s, T& t) { s.serialize(t);                 } &&
    !requires   (S s      ) { s.serialize(std::declval<T>()); };


template <template <typename, bool> class... Handlers>
struct handler_list
{
private:
    template <IsHandlerList HandlerList>
    struct extend_impl;

    template <template <typename, bool> class... OtherHandlers>
    struct extend_impl<handler_list<OtherHandlers...>>
    {
        using type = handler_list<Handlers..., OtherHandlers...>;
    };

    template <typename TSerializer, bool IsReading, typename T>
    static consteval std::meta::info get_first()
    {
        // Go trough all Handlers
        template for (constexpr auto template_param : std::define_static_array(std::meta::template_arguments_of(^^handler_list)))
        {
            using THandler = [: template_param :]<TSerializer, IsReading>;
            using THandlerBase = custom_type_handler<TSerializer, IsReading>;

            static_assert
            (
                std::derived_from<THandler, THandlerBase>, 
                "Ill-formed type handler encountered. It should derive from custom_type_handler with forwarded template params!"
            );

            static_assert
            (
                std::is_default_constructible_v<THandler>, 
                "Ill-formed type handler encountered. It should have default constructor!"
            );

            if constexpr (HasHandlerFor<THandler, T>)
            {
                return ^^THandler;
            }
        }
        
        assert(false && "Could not find handler for type");
    };

public:
    template <IsHandlerList HandlerList>
    using extend = extend_impl<HandlerList>::type;

    template <template <typename, bool> class Handler>
    using append = handler_list<Handlers..., Handler>;

    template <typename TSerializer, bool IsReading, typename T>
    using get_first_t = [: get_first<TSerializer, IsReading, T>() :];
};

} // reflexx

#endif