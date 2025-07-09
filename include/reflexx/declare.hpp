#ifndef REFLEXX_DECLARE_HPP
#define REFLEXX_DECLARE_HPP

#include "reflexx/concepts/backend.hpp"
#include "reflexx/serializer_settings.hpp"

namespace reflexx {

using namespace ::reflexx::concepts;

template <template <typename, bool> class... Handlers>
struct handler_list;

namespace detail {

    template <typename T>
    struct is_handler_list : std::false_type {};

    template <template <typename, bool> class... Handlers>
    struct is_handler_list<::reflexx::handler_list<Handlers...>> : std::true_type {};

    template <typename T>
    static constexpr bool is_handler_list_v = is_handler_list<T>::value;

} // detail

template <typename T>
concept IsHandlerList = ::reflexx::detail::is_handler_list_v<T>;

template <serializer_settings S, IsBackendType B, IsHandlerList D>
class serializer;

namespace detail {

    template <typename T>
    struct is_serializer : std::false_type {};
    
    template <serializer_settings S, IsBackendType B, IsHandlerList D>
    struct is_serializer<serializer<S, B, D>> : std::true_type {};

    template <typename T>
    static constexpr bool is_serializer_v = is_serializer<T>::value;

} // detail

template <typename T>
concept IsSerializer = ::reflexx::detail::is_serializer<T>::value;


} // reflexx

#endif