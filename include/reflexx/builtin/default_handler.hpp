#ifndef REFLEXX_DEFAULT_TYPE_HANDLER_HPP
#define REFLEXX_DEFAULT_TYPE_HANDLER_HPP

#include <experimental/meta>

#include "reflexx/serializer_settings.hpp"
#include "reflexx/type_handler.hpp"
#include "reflexx/annotations.hpp"
#include "reflexx/util/non_serializable_category_type.hpp"

namespace reflexx {
namespace detail {

using namespace annotations;

consteval std::string_view resolve_member_name(std::meta::info member_info)
{
    auto name = get_annotated_name(member_info);
    auto parent_type_info = std::meta::parent_of(member_info);

    if (has_enabled_annotations(parent_type_info) && name.has_value())
    {
        return *name;
    }
    else
    {
        return std::meta::identifier_of(member_info);
    }
}

template <serializer_settings Settings, std::meta::info MemberInfo>
requires (std::meta::is_nonstatic_data_member(MemberInfo))
consteval bool should_handle_member()
{
    using member_t = typename[: std::meta::type_of(MemberInfo) :];
    constexpr auto parent_type_info = std::meta::parent_of(MemberInfo);

    // We manually ignoring this field
    if (has_enabled_annotations(parent_type_info) && has_ignore_annotation(MemberInfo))
    {
        return false;
    }

    // Otherwise, we will ignore non serializable category types
    if constexpr (util::is_non_serializable_category_type_v<member_t>)
    {
        static_assert(ignore_non_serializable_members_v<Settings>, "Non serializable category type member encountered!");
        return false;
    }

    // And we will also ignore constant fields
    // NOTE: Why? Serialization clearly does not modify it?
    //  Reasons are multiple. First off, it will make serialization and deserialization identical
    //  Other reason is that by design, there is no point writing it if you wont be able to read it!
    //  "But my frontend will read it, not this program" 
    //  -> then if you can read it into non const field on frontend, why is it marked const on backend?
    if constexpr (std::is_const_v<member_t>)
    {
        static_assert(ignore_non_serializable_members_v<Settings>, "Const member encountered!");
        return false;
    }

    // Fallback -> we should serialize
    return true;
};

template <typename T>
requires std::is_class_v<T>
consteval bool has_ambigous_non_static_data_members()
{
    std::vector<std::string_view> member_names {};
    std::vector<std::meta::info> slice_types { ^^T };

    while (!slice_types.empty())
    {
        const auto slice_type = slice_types.back();
        slice_types.pop_back();

        for (const auto& member_info : std::meta::nonstatic_data_members_of(slice_type, std::meta::access_context::unchecked()))
        {
            const auto& member_name = resolve_member_name(member_info);
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

template <typename T>
requires std::is_class_v<T>
consteval bool has_virtual_base()
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
concept ValidateClass =
    !has_ambigous_non_static_data_members<T>() &&
    !has_virtual_base<T>();


} // detail

using namespace detail;

template <typename TSerializer, bool IsReading>
struct default_handler : type_handler<TSerializer, IsReading>
{
    template <typename T>
    void serialize(T& obj)
    {
        static_assert(ValidateClass<T>, "Checks not satisfied!");

        this->begin_object();

        serialize_slice(obj);

        this->end_object();
    }

    template <typename S>
    void serialize_slice(S& obj)
    {
        std::byte* obj_ptr = reinterpret_cast<std::byte*>(&obj);

        template for (constexpr auto& base_info : std::define_static_array(std::meta::bases_of(^^S, std::meta::access_context::unchecked())))
        {
            using TBase = typename [: std::meta::type_of(base_info) :];

            constexpr std::ptrdiff_t slice_offset = std::meta::offset_of(base_info).bytes;
            std::byte* base_ptr = obj_ptr + slice_offset;
            serialize_slice<TBase>(*reinterpret_cast<TBase*>(base_ptr));
        }

        template for (constexpr auto& member_info : std::define_static_array(std::meta::nonstatic_data_members_of(^^S, std::meta::access_context::unchecked())))
        {
            if constexpr (should_handle_member<TSerializer::settings, member_info>())
            {
                this->key(resolve_member_name(member_info));
                this->serialize_object(obj.[: member_info :]);
            }
        }
    }

};


} // reflexx

#endif