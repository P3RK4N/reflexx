#ifndef REFLEXX_POLICIES_HPP
#define REFLEXX_POLICIES_HPP

// TODO: Greedyness
// TODO: Marching policy (by order or by key)

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

} // reflexx::policies

#endif