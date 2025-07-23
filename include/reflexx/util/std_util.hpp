#ifndef REFLEXX_STD_UTIL_HPP
#define REFLEXX_STD_UTIL_HPP

#include <type_traits>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace reflexx {
namespace util {
namespace detail {

template<typename T>
struct is_std_map : std::false_type {};

template<typename K, typename V, typename... Args>
struct is_std_map<std::map<K, V, Args...>> : std::true_type {};

template<typename K, typename V, typename... Args>
struct is_std_map<std::multimap<K, V, Args...>> : std::true_type {};

template<typename K, typename V, typename... Args>
struct is_std_map<std::unordered_map<K, V, Args...>> : std::true_type {};

template<typename K, typename V, typename... Args>
struct is_std_map<std::unordered_multimap<K, V, Args...>> : std::true_type {};

template<typename T>
struct is_std_set : std::false_type {};

template<typename K, typename... Args>
struct is_std_set<std::set<K, Args...>> : std::true_type {};

template<typename K, typename... Args>
struct is_std_set<std::multiset<K, Args...>> : std::true_type {};

template<typename K, typename... Args>
struct is_std_set<std::unordered_set<K, Args...>> : std::true_type {};

template<typename K, typename... Args>
struct is_std_set<std::unordered_multiset<K, Args...>> : std::true_type {};

} // detail

template <typename T>
using is_std_map = detail::is_std_map<std::remove_cvref_t<T>>;

template<typename T>
constexpr bool is_std_map_v = is_std_map<T>::value;

template <typename T>
using is_std_set = detail::is_std_set<std::remove_cvref_t<T>>;

template<typename T>
constexpr bool is_std_set_v = is_std_set<std::remove_cvref_t<T>>::value;

} // util
} // reflexx

#endif