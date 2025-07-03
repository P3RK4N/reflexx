#ifndef REFLEXX_ENUM_CONV_HPP
#define REFLEXX_ENUM_CONV_HPP

#include <experimental/meta>
#include <string_view>
#include <stdexcept>
#include <type_traits>

namespace reflexx {
namespace util {
    
template<typename E, bool Enumerable = std::meta::is_enumerable_type(^^std::remove_cvref_t<E>)>
requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr std::string_view enum_to_string(E value)
{
    if constexpr (Enumerable)
    {
        template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^std::remove_cvref_t<E>)))
        {
            if (value == [: e :])
            {
                return std::meta::identifier_of(e);
            }
        }

        throw std::invalid_argument("Invalid enum value!");
    }
    else
    {
        static_assert(false, "Not an enumerable type!");
    };
}

template <typename E, bool Enumerable = std::meta::is_enumerable_type(^^std::remove_cvref_t<E>)>
requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr E string_to_enum(std::string_view name)
{
    if constexpr (Enumerable)
    {
        template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^std::remove_cvref_t<E>)))
        {
            if (name == std::meta::identifier_of(e))
            {
                return [: e :];
            }
        }

        throw std::invalid_argument("Invalid enum name: " + std::string(name));
    }
    else
    {
        static_assert(false, "Not an enumerable type!");
    };
}


} // util
} // reflexx

#endif