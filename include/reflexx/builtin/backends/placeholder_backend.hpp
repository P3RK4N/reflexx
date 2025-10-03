#ifndef REFLEXX_PLACEHOLDER_BACKEND_HPP
#define REFLEXX_PLACEHOLDER_BACKEND_HPP

#include <string_view>
#include <cstddef>
#include <span>

#include "reflexx/util/serializable.hpp"

namespace reflexx::backends {

// Empty constexpr interface
struct placeholder_backend
{
    inline constexpr explicit placeholder_backend()                         {               }
    inline constexpr explicit placeholder_backend(std::span<char>)          {               }
    inline constexpr explicit placeholder_backend(std::span<const char>)    {               }
    inline constexpr ~placeholder_backend()                                 {               }

    inline constexpr std::string_view  get()                                { return {};    }

    template <typename T>
    requires util::is_serializable_number_v<T>
    inline constexpr void              write_number(T)                      {               }
    inline constexpr void              write_key(std::string_view)          {               }
    inline constexpr void              write_begin_array()                  {               }
    inline constexpr void              write_end_array()                    {               }
    inline constexpr void              write_begin_object()                 {               }
    inline constexpr void              write_end_object()                   {               }
    inline constexpr void              write_char(char)                     {               }
    inline constexpr void              write_bool(bool)                     {               }
    inline constexpr void              write_string(std::string_view)       {               }
    inline constexpr void              write_null()                         {               }

    template <typename T>
    requires util::is_serializable_number_v<T>
    inline constexpr void              read_number(T&)                      {               }
    inline constexpr bool              read_key(std::string_view)           { return true;  }
    inline constexpr std::string_view  read_key()                           { return {};    }
    inline constexpr void              read_begin_array()                   {               }
    inline constexpr void              read_end_array()                     {               }
    inline constexpr void              read_begin_object()                  {               }
    inline constexpr void              read_end_object()                    {               }
    inline constexpr void              read_char(char&)                     {               }
    inline constexpr void              read_bool(bool&)                     {               }
    inline constexpr std::string_view  read_string()                        { return "";    }
    inline constexpr bool              read_is_null()                       { return false; }
    inline constexpr void              read_skip()                          {               }
    inline constexpr bool              read_has_next()                      { return false; }
};

} // reflexx::backends

#endif
