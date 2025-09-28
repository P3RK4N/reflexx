#ifndef REFLEXX_SERIALIZABLE_STRING_HPP
#define REFLEXX_SERIALIZABLE_STRING_HPP

#include <type_traits>
#include <string>
#include <string_view>

#include "serializable_char.hpp"

namespace reflexx::util {
namespace detail {

template <typename T>
struct is_serializable_string : std::false_type {};

template <is_serializable_char CharT, typename Traits, typename Alloc>
struct is_serializable_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

template <is_serializable_char CharT, typename Traits>
struct is_serializable_string<std::basic_string_view<CharT, Traits>> : std::true_type {};

} // detail

template <typename T>
static inline constexpr bool is_serializable_string_v = detail::is_serializable_string<std::remove_cvref_t<T>>::value;

template <typename T>
concept is_serializable_string = is_serializable_string_v<T>;

} // reflexx::util

#endif