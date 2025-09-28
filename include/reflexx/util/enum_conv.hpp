#ifndef REFLEXX_ENUM_CONV_HPP
#define REFLEXX_ENUM_CONV_HPP

#include <meta>
#include <string_view>
#include <cassert>

#include "reflexx/util/serializable_enum.hpp"

namespace reflexx::util {
    
template<is_serializable_enum E>
static inline constexpr std::string_view enum_to_string(E value) noexcept
{
    template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^std::remove_cvref_t<E>)))
    {
        if (value == [: e :])
        {
            return std::meta::identifier_of(e);
        }
    }

    assert(false && "Invalid enum value");
}

template <is_serializable_enum E>
static inline constexpr E string_to_enum(std::string_view name) noexcept
{
    template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^std::remove_cvref_t<E>)))
    {
        if (name == std::meta::identifier_of(e))
        {
            return [: e :];
        }
    }

    assert(false && "Invalid enum name");
}

} // reflexx::util

#endif