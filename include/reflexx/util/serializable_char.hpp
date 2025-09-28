#ifndef REFLEXX_SERIALIZABLE_CHAR_HPP
#define REFLEXX_SERIALIZABLE_CHAR_HPP

#include <type_traits>

// NOTE: Unsigned char and signed chars are not "chars"
// https://en.cppreference.com/w/cpp/language/type-id.html

namespace reflexx::util {
namespace detail {

template <typename T>
static inline constexpr bool is_serializable_char_v =
    std::is_same_v<T, char> || 
    std::is_same_v<T, char8_t>; // UTF-8 only

} // detail

template <typename T>
static inline constexpr bool is_serializable_char_v = detail::is_serializable_char_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_char = is_serializable_char_v<T>;

} // reflexx::util

#endif