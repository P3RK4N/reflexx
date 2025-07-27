#ifndef REFLEXX_TYPE_HANDLER_LIST_HPP
#define REFLEXX_TYPE_HANDLER_LIST_HPP

#include <cassert>
#include <experimental/meta>
#include <tuple>
#include <utility>

#include "declare.hpp"

namespace reflexx {

template <typename S, typename T>
concept HasHandlerFor =
    requires    (S s, T& t) { s.serialize(t);                 } &&
    !requires   (S s      ) { s.serialize(std::declval<T>()); };


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

    template <typename TSerializer, bool IsReading, typename T>
    static consteval std::size_t get_first_index()
    {
        std::size_t index = 0;

        // Go trough all Handlers
        template for (constexpr auto template_param : std::define_static_array(std::meta::template_arguments_of(^^type_handler_list)))
        {
            using THandler = [: template_param :]<TSerializer, IsReading>;

            if constexpr (HasHandlerFor<THandler, T>)
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

    template <typename TSerializer, bool IsReading, typename T>
    static constexpr std::size_t get_first_index_v = get_first_index<TSerializer, IsReading, T>();
};

} // reflexx

#endif