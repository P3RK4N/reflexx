#ifndef REFLEXX_PROVIDER_HPP
#define REFLEXX_PROVIDER_HPP

#include <type_traits>

namespace reflexx {

template <typename T>
struct provider
{
    inline constexpr T operator()() const
    {
        static_assert(std::is_default_constructible_v<T>, "Type is not default constructible, consider specializing provider for it!");
        return {};
    }
};

} // reflexx

#endif