#ifndef REFLEXX_SERIALIZER_SETTINGS_HPP
#define REFLEXX_SERIALIZER_SETTINGS_HPP

#include <cassert>
#include <experimental/meta>
#include <type_traits>

#include "reflexx/policies.hpp"
#include "reflexx/util/non_serializable_category_type.hpp"

namespace reflexx
{
    
using namespace ::reflexx::policies;

#define DECLARE_POLICY(policy_name)                                         \
    policy_name policy_name##_;                                             \
    bool policy_name##_set_ = false;                                        \
    consteval serializer_settings& set_##policy_name(policy_name policy)    \
    {                                                                       \
        assert(!policy_name##_set_);                                        \
        policy_name##_ = policy;                                            \
        policy_name##_set_ = true;                                          \
        return *this;                                                       \
    }

#define DECLARE_PROPERTY(type, name)                                        \
    type name##_ = type();                                                  \
    bool name##_set_ = false;                                               \
    consteval serializer_settings& set_##name(type value)                   \
    {                                                                       \
        assert(!name##_set_);                                               \
        name##_ = value;                                                    \
        name##_set_ = true;                                                 \
        return *this;                                                       \
    }   

struct serializer_settings final
{
    static inline consteval serializer_settings Strict()
    {
        serializer_settings settings;

        settings.enum_format_policy_                = enum_format_policy::integral;
        settings.handler_matching_policy_           = handler_matching_policy::exact;
        settings.ignore_non_serializable_members_   = false;

        return settings; 
    }
    
    static inline consteval serializer_settings Relaxed()
    {
        serializer_settings settings;

        settings.enum_format_policy_                = enum_format_policy::string;
        settings.handler_matching_policy_           = handler_matching_policy::callable;
        settings.ignore_non_serializable_members_   = true;

        return settings;
    }

    DECLARE_POLICY(enum_format_policy);
    DECLARE_POLICY(handler_matching_policy);
    
    DECLARE_PROPERTY(bool, ignore_non_serializable_members);

private:
    consteval serializer_settings() = default;
};

#undef DECLARE_POLICY
#undef DECLARE_PROPERTY

namespace util {

    template <serializer_settings Settings>
static constexpr bool format_enum_as_string_v =
    Settings.enum_format_policy_ == policies::enum_format_policy::string;

template <serializer_settings Settings>
static constexpr bool use_exact_handler_matching_v =
    Settings.handler_matching_policy_ == policies::handler_matching_policy::exact;

template <serializer_settings Settings>
static constexpr bool ignore_non_serializable_members_v =
    Settings.ignore_non_serializable_members_;

/*
    If we are not ignoring anything, we should definitively try to handle member.
    Otherwise, if we ignore non serializable members, we will handle only
    those that are not const and not non-serializable members.
*/
template <serializer_settings Settings, std::meta::info MemberInfo>
requires (std::meta::is_nonstatic_data_member(MemberInfo))
static constexpr bool should_handle_member_v =
    !ignore_non_serializable_members_v<Settings> ||
    (
        !util::is_non_serializable_category_type_v<typename[: std::meta::type_of(MemberInfo) :]> &&
        !std::is_const_v<typename[: std::meta::type_of(MemberInfo) :]>
    );

} // util
} // reflexx

#endif