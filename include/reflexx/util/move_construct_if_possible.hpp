#ifndef REFLEXX_MOVE_CONSTRUCT_IF_POSSIBLE_HPP
#define REFLEXX_MOVE_CONSTRUCT_IF_POSSIBLE_HPP

#include <type_traits>
#include <utility>

namespace reflexx {
namespace util {

template <typename T>
inline constexpr decltype(auto) move_construct_if_possible(T& arg) noexcept
{
    if constexpr (std::is_move_constructible_v<T>)
    {
        return std::move(arg);
    }
    else
    {
        return arg;
    }
}

} // util
} // reflexx

#endif