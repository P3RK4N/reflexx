#ifndef REFLEXX_POLICIES_HPP
#define REFLEXX_POLICIES_HPP

// TODO: Greedyness
// TODO: Marching policy (by order or by key)
// TODO: Emit null field policy

namespace reflexx::policies {

enum class enum_format_policy
{
    string,
    integral
};

enum class handler_matching_policy
{
    exact,
    callable
};

enum class missing_field_policy
{
    disallow,
    treat_as_null
};

} // reflexx::policies

#endif