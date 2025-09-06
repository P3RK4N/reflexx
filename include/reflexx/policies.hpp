#ifndef REFLEXX_POLICIES_HPP
#define REFLEXX_POLICIES_HPP

namespace reflexx {
namespace policies {

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

} // policies
} // reflexx

#endif