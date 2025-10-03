#ifndef REFLEXX_SERIALIZABLE_HPP
#define REFLEXX_SERIALIZABLE_HPP

#include <type_traits>
#include <string>
#include <string_view>
#include <meta>

namespace reflexx::util {
namespace detail {

template <typename T>
static inline constexpr bool is_serializable_char_v =
    std::is_same_v<T, char> || 
    std::is_same_v<T, char8_t>; // We allow UTF-8 only

template <typename T>
static inline constexpr bool is_serializable_number_v =
    std::is_same_v<T, std::int8_t>                      || 
    std::is_same_v<T, std::int16_t>                     ||
    std::is_same_v<T, std::int32_t>                     ||
    std::is_same_v<T, std::int64_t>                     ||
    std::is_same_v<T, std::uint8_t>                     ||
    std::is_same_v<T, std::uint16_t>                    ||
    std::is_same_v<T, std::uint32_t>                    ||
    std::is_same_v<T, std::uint64_t>                    || 
    (std::is_same_v<T, float>  && sizeof(float)  == 4)  || // TODO: Update with <stdfloat>, until then "force" serializables to have fixed size.
    (std::is_same_v<T, double> && sizeof(double) == 8);

template <typename T>
static inline constexpr bool is_serializable_enum_v =
    std::is_enum_v<T> &&
    std::meta::is_enumerable_type(^^T);

} // detail


// NOTE: Unsigned char and signed chars are not "chars"
// https://en.cppreference.com/w/cpp/language/type-id.html

template <typename T>
static inline constexpr bool is_serializable_char_v     = detail::is_serializable_char_v<std::remove_cvref_t<T>>;

template <typename T>
static inline constexpr bool is_serializable_number_v   = detail::is_serializable_number_v<std::remove_cvref_t<T>>;

template <typename T>
static inline constexpr bool is_serializable_enum_v     = detail::is_serializable_enum_v<std::remove_cvref_t<T>>;

template <typename T>
static inline constexpr bool is_serializable_bool_v     = std::is_same_v<std::remove_cvref_t<T>, bool>;

template <typename T>
static inline constexpr bool is_serializable_array_v    = std::is_bounded_array_v<std::remove_cvref_t<T>>;

template <typename T>
static inline constexpr bool is_serializable_nullptr_v  = std::is_null_pointer_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_char                            = is_serializable_char_v<T>;

template <typename T>
concept is_serializable_number                          = is_serializable_number_v<T>;

template <typename T>
concept is_serializable_enum                            = is_serializable_enum_v<T>;

template <typename T>
concept is_serializable_bool                            = is_serializable_bool_v<T>;

template <typename T>
concept is_serializable_array                           = is_serializable_array_v<T>;

template <typename T>
concept is_serializable_nullptr                         = is_serializable_nullptr_v<T>;

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
static inline constexpr bool is_serializable_class_v = 
    std::is_class_v<std::remove_cvref_t<T>> &&
    !is_serializable_string_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_string = is_serializable_string_v<T>;

template <typename T>
concept is_serializable_class = is_serializable_class_v<T>;

template <typename T>
concept is_non_serializable_type =
    std::is_void_v<T>           ||
    std::is_union_v<T>          ||
    std::is_reference_v<T>      ||
    std::is_pointer_v<T>        ||
    std::is_member_pointer_v<T> ||
    std::is_function_v<T>;

template <typename T>
concept is_serializable_type =
    !is_non_serializable_type<T>     && // Provides a better error message 
    (
        is_serializable_nullptr<T> ||
        is_serializable_char<T>    ||
        is_serializable_number<T>  ||
        is_serializable_enum<T>    ||
        is_serializable_bool<T>    ||
        is_serializable_string<T>  ||
        is_serializable_array<T>   ||
        is_serializable_class<T>
    );


} // reflexx::util

#endif