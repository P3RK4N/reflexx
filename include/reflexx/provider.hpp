#ifndef REFLEXX_PROVIDER_HPP
#define REFLEXX_PROVIDER_HPP

#include <type_traits>

namespace reflexx {

template <typename T>
struct provider
{
    inline constexpr T operator()() const noexcept(std::is_nothrow_default_constructible_v<std::remove_cvref_t<T>>)
    {
        return {};
    }
};

} // reflexx

#endif