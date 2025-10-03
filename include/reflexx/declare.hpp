#ifndef REFLEXX_DECLARE_HPP
#define REFLEXX_DECLARE_HPP

#include "reflexx/backend.hpp"
#include "reflexx/util/serializable.hpp"
#include "reflexx/serializer_settings.hpp"

namespace reflexx {

template <template <typename, bool> class... Handlers>
struct type_handler_list;

namespace detail {

    template <typename T>
    struct is_type_handler_list : std::false_type {};

    template <template <typename, bool> class... Handlers>
    struct is_type_handler_list<::reflexx::type_handler_list<Handlers...>> : std::true_type {};

    template <typename T>
    static constexpr bool is_type_handler_list_v = is_type_handler_list<T>::value;

} // detail

template <typename T>
concept IsTypeHandlerList = ::reflexx::detail::is_type_handler_list_v<T>;

template <serializer_settings S, IsBackendType B, IsTypeHandlerList D>
class serializer;

namespace detail {

    template <typename T>
    struct is_serializer : std::false_type {};
    
    template <serializer_settings S, IsBackendType B, IsTypeHandlerList D>
    struct is_serializer<serializer<S, B, D>> : std::true_type {};

    template <typename T>
    static constexpr bool is_serializer_v = is_serializer<T>::value;

} // detail

template <typename T>
concept IsSerializer = ::reflexx::detail::is_serializer<T>::value;


} // reflexx

#endif