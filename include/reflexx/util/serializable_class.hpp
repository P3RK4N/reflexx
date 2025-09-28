#ifndef REFLEXX_SERIALIZABLE_CLASS_HPP
#define REFLEXX_SERIALIZABLE_CLASS_HPP

#include <type_traits>
#include "serializable_string.hpp"

namespace reflexx::util {
namespace detail {

template <typename T>
static inline constexpr bool is_serializable_class_v = std::is_class_v<T> && !is_serializable_string_v<T>;

} // detail

template <typename T>
static inline constexpr bool is_serializable_class_v = detail::is_serializable_class_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_class = is_serializable_class_v<T>;

} // reflexx::util

#endif