#ifndef REFLEXX_CLASS_UTIL_HPP
#define REFLEXX_CLASS_UTIL_HPP

#include <experimental/meta>
#include <type_traits>
#include <vector>
#include <cassert>

namespace reflexx {
namespace util {
namespace detail {

template <typename T>
requires std::is_class_v<T>
consteval bool has_virtual_base_impl()
{
    std::vector<std::meta::info> slice_types { ^^T };

    while (!slice_types.empty())
    {
        const auto slice_type = slice_types.back();
        slice_types.pop_back();

        for (const auto& base : std::meta::bases_of(slice_type, std::meta::access_context::unchecked()))
        {
            if (std::meta::is_virtual(base))
            {
                return true;
            }

            slice_types.emplace_back(std::meta::type_of(base));
        }
    }

    return false;
}

template <typename T>
requires std::is_class_v<T>
consteval bool has_ambigous_non_static_data_members_impl()
{
    std::vector<std::string_view> member_names {};
    std::vector<std::meta::info> slice_types { ^^T };

    while (!slice_types.empty())
    {
        const auto slice_type = slice_types.back();
        slice_types.pop_back();

        for (const auto& member : std::meta::nonstatic_data_members_of(slice_type, std::meta::access_context::unchecked()))
        {
            const auto& member_name = std::meta::identifier_of(member);
            if (std::find(member_names.begin(), member_names.end(), member_name) != member_names.end())
            {
                return true;
            }

            member_names.emplace_back(member_name);
        }

        for (const auto& base : std::meta::bases_of(slice_type, std::meta::access_context::unchecked()))
        {
            slice_types.emplace_back(std::meta::type_of(base));
        }
    }

    return false;
}

} // detail

template <typename T>
static constexpr bool has_virtual_base_v = detail::has_virtual_base_impl<std::remove_cvref_t<T>>();

template <typename T>
static constexpr bool has_ambigous_non_static_data_members_v = detail::has_ambigous_non_static_data_members_impl<std::remove_cvref_t<T>>();

} // util
} // reflexx

#endif