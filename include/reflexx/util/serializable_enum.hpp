#ifndef REFLEXX_SERIALIZABLE_ENUM_HPP
#define REFLEXX_SERIALIZABLE_ENUM_HPP

#include <meta>
#include <type_traits>

namespace reflexx::util {
namespace detail {

template <typename T>
static inline constexpr bool is_serializable_enum_v =
    std::is_enum_v<T> &&
    std::meta::is_enumerable_type(^^T);

} // detail

template <typename T>
static inline constexpr bool is_serializable_enum_v = detail::is_serializable_enum_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_enum = is_serializable_enum_v<T>;

} // reflexx::util

#endif