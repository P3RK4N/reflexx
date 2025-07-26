#ifndef REFLEXX_IS_SUPPORTED_NUMBER_HPP
#define REFLEXX_IS_SUPPORTED_NUMBER_HPP

#include <type_traits>

namespace reflexx {
namespace util {

// TODO: Update with <stdfloat>

template <typename T>
inline constexpr bool is_serializable_number_v =
       std::is_same_v<T, std::int8_t>
    || std::is_same_v<T, std::int16_t>
    || std::is_same_v<T, std::int32_t>
    || std::is_same_v<T, std::int64_t>
    || std::is_same_v<T, std::uint8_t>
    || std::is_same_v<T, std::uint16_t>
    || std::is_same_v<T, std::uint32_t>
    || std::is_same_v<T, std::uint64_t>
    || std::is_same_v<T, float>
    || std::is_same_v<T, double>;

template <typename T>
concept SerializableNumber = is_serializable_number_v<T>;

} // util
} // reflexx


#endif