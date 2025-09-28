#ifndef REFLEXX_IS_SUPPORTED_NUMBER_HPP
#define REFLEXX_IS_SUPPORTED_NUMBER_HPP

#include <type_traits>

namespace reflexx::util {
namespace detail {
    
template <typename T>
static inline constexpr bool is_serializable_number_v =
    std::is_same_v<T, std::int8_t>   || 
    std::is_same_v<T, std::int16_t>  ||
    std::is_same_v<T, std::int32_t>  ||
    std::is_same_v<T, std::int64_t>  ||
    std::is_same_v<T, std::uint8_t>  ||
    std::is_same_v<T, std::uint16_t> ||
    std::is_same_v<T, std::uint32_t> ||
    std::is_same_v<T, std::uint64_t> || 
    std::is_same_v<T, float>         || // TODO: Update with <stdfloat>
    std::is_same_v<T, double>;

} // detail

template <typename T>
static inline constexpr bool is_serializable_number_v = detail::is_serializable_number_v<std::remove_cvref_t<T>>;

template <typename T>
concept is_serializable_number = is_serializable_number_v<T>;

} // reflexx::util


#endif