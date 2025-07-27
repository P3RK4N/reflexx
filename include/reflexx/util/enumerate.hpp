#ifndef REFLEXX_ENUMERATE_HPP
#define REFLEXX_ENUMERATE_HPP

#include <experimental/meta>
#include <span>

namespace reflexx {
namespace util {

template <std::size_t N>
consteval static std::span<const std::size_t> enumerate()
{
    std::array<std::size_t, N> arr;

    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = i;
    }

    return std::define_static_array(arr);
}    

} // util
} // reflexx

#endif