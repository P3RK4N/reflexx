#ifndef REFLEXX_CHECKS_HPP
#define REFLEXX_CHECKS_HPP

#include "serializer_settings.hpp"
#include "util/class_util.hpp"

namespace reflexx {
namespace concepts {

using namespace ::reflexx::util;

template <typename T>
concept AmbiguousMemberCheck =
    !has_ambigous_non_static_data_members_v<T>;

template <typename T>
concept VirtualBaseClassCheck =
    !has_virtual_base_v<T>;

template <serializer_settings Settings, typename T>
concept ValidationCheck =
    AmbiguousMemberCheck<T> &&
    VirtualBaseClassCheck<T>;

} // concepts
} // reflexx

#endif