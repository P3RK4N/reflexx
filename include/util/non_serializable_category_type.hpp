#ifndef REFLEXX_NON_SERIALIZABLE_CATEGORY_TYPE_HPP
#define REFLEXX_NON_SERIALIZABLE_CATEGORY_TYPE_HPP

#include <type_traits>

namespace reflexx {
namespace util {

template <typename T>
static constexpr bool is_non_serializable_category_type_v =
    std::is_reference_v<T>          ||
    std::is_pointer_v<T>            ||
    std::is_member_pointer_v<T>     ||
    std::is_unbounded_array_v<T>    ||
    std::is_function_v<T>           ||
    std::is_union_v<T>;

} // util
} // reflexx

#endif