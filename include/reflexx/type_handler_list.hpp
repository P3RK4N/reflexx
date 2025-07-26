#ifndef REFLEXX_TYPE_HANDLER_LIST_HPP
#define REFLEXX_TYPE_HANDLER_LIST_HPP

#include <cassert>
#include <concepts>
#include <experimental/meta>
#include <utility>

#include "reflexx/type_handler.hpp"

namespace reflexx {

template <typename S, typename T>
concept HasHandlerFor =
    requires    (S s, T& t) { s.serialize(t);                 } &&
    !requires   (S s      ) { s.serialize(std::declval<T>()); };


template <template <typename, bool> class... Handlers>
struct type_handler_list
{
private:
    template <IsTypeHandlerList TypeHandlerList>
    struct extend_impl;

    template <template <typename, bool> class... OtherHandlers>
    struct extend_impl<type_handler_list<OtherHandlers...>>
    {
        using type = type_handler_list<Handlers..., OtherHandlers...>;
    };

    template <typename TSerializer, bool IsReading, typename T>
    static consteval std::meta::info get_first()
    {
        // Go trough all Handlers
        template for (constexpr auto template_param : std::define_static_array(std::meta::template_arguments_of(^^type_handler_list)))
        {
            using THandler = [: template_param :]<TSerializer, IsReading>;

            if constexpr (HasHandlerFor<THandler, T>)
            {
                return ^^THandler;
            }
        }
        
        assert(false && "Could not find handler for type");
        std::unreachable();
    };

public:
    template <IsTypeHandlerList TypeHandlerList>
    using extend = extend_impl<TypeHandlerList>::type;

    template <template <typename, bool> class Handler>
    using append = type_handler_list<Handlers..., Handler>;

    template <typename TSerializer, bool IsReading, typename T>
    using get_first_t = [: get_first<TSerializer, IsReading, T>() :];
};

} // reflexx

#endif