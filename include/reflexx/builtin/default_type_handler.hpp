#ifndef REFLEXX_DEFAULT_TYPE_HANDLER_HPP
#define REFLEXX_DEFAULT_TYPE_HANDLER_HPP

#include <experimental/meta>

#include "reflexx/concepts/checks.hpp"
#include "reflexx/custom_type_handler.hpp"

namespace reflexx {

using namespace ::reflexx::concepts;

template <typename TSerializer, bool IsReading>
struct default_type_handler : custom_type_handler<TSerializer, IsReading>
{
    template <typename T>
    void serialize(T& obj)
    {
        static_assert(ValidationCheck<TSerializer::settings, T>, "Checks not satisfied!");

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
            using TMember = [: std::meta::type_of(member_info) :];

            if constexpr (should_handle_member_v<TSerializer::settings, member_info>)
            {
                assert_member_type<member_info>();
                
                this->key(std::meta::identifier_of(member_info));
                this->serialize_object(obj.[: member_info :]);
            }
        }
    }

    template <std::meta::info MemberInfo>
    requires (std::meta::is_nonstatic_data_member(MemberInfo))
    static void assert_member_type()
    {
        using TMember = typename[: std::meta::type_of(MemberInfo) :];

        static_assert
        (
            !std::is_const_v<TMember> && !std::is_reference_v<TMember>,
            "\n\n\n"
            "##########################################################\n"
            "############ ˇDEFAULT TYPE HANDLER ERRORˇ ################\n"
            "##########################################################\n"
            "\n\n"
            "Nonstatic data members cannot be const or reference types!\n" 
            "Consider writing custom type handler for it.\n"
            "Note that deserialization into const types is undefined\n"
            "behaviour.\n"
            "\n\n"
            "##########################################################\n"
            "############ ^DEFAULT TYPE HANDLER ERROR^ ################\n"
            "##########################################################\n"
            "\n\n\n"
        );
    }

};


} // reflexx

#endif