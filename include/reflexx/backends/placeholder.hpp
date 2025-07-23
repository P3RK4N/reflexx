#ifndef REFLEXX_PLACEHOLDER_BACKEND_HPP
#define REFLEXX_PLACEHOLDER_BACKEND_HPP

#include <string_view>
#include <cstddef>
#include <span>
#include <type_traits>
#include <cassert>

namespace reflexx {
namespace backends {

// Empty constexpr interface
struct placeholder_backend
{
    constexpr placeholder_backend()                             {               }
    constexpr placeholder_backend(std::span<const char>)        {               }
    constexpr ~placeholder_backend()                            {               }

    constexpr std::string_view  get()                           { return {};    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    constexpr void              write_number(T)                 {               }
    constexpr void              write_key(std::string_view)     {               }
    constexpr void              write_begin_array()             {               }
    constexpr void              write_end_array()               {               }
    constexpr void              write_begin_object()            {               }
    constexpr void              write_end_object()              {               }
    constexpr void              write_bool(bool)                {               }
    constexpr void              write_string(std::string_view)  {               }
    constexpr void              write_null()                    {               }

    template <typename T>
    requires std::is_arithmetic_v<T>
    constexpr void              read_number(T&)                 {               }
    constexpr void              read_key(std::string_view)      {               }
    constexpr std::string_view  read_key()                      { return {};    }
    constexpr void              read_begin_array()              {               }
    constexpr void              read_end_array()                {               }
    constexpr void              read_begin_object()             {               }
    constexpr void              read_end_object()               {               }
    constexpr void              read_bool(bool&)                {               }
    constexpr std::string_view  read_string()                   { return "";    }
    constexpr bool              read_is_null()                  { return false; }
    constexpr void              read_skip()                     {               }
    constexpr bool              read_has_next()                 { return false; }
};

} // backends
} // reflexx

#endif
