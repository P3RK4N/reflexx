#ifndef REFLEXX_DATA_UTIL_HPP
#define REFLEXX_DATA_UTIL_HPP

#include <concepts>
#include <map>
#include <type_traits>
#include <vector>
#include <string_view>

#include <yyjson.h>

#include <reflexx/builtin/backends/yyjson_backend.hpp>

using namespace reflexx::backends;

inline void deserialize(std::string& out, yyjson_val* val)
{
    out = yyjson_get_str(val);
}

inline void deserialize(std::string& out, yyjson_backend& backend)
{
    out = std::string(backend.read_string());
}

inline void deserialize(std::string_view& out, yyjson_val* val)
{
    out = { yyjson_get_str(val), yyjson_get_len(val) };
}

inline void deserialize(std::string_view& out, yyjson_backend& backend)
{
    out = backend.read_string();
}

template <std::integral T>
inline void deserialize(T& out, yyjson_val* val)
{
    if constexpr (std::is_signed_v<T>)
        out = static_cast<T>(yyjson_get_sint(val));
    else
        out = static_cast<T>(yyjson_get_uint(val));
}

template <std::integral T>
inline void deserialize(T& out, yyjson_backend& backend)
{
    backend.read_number(out);
}

template <std::floating_point T>
inline void deserialize(T& out, yyjson_val* val)
{
    out = static_cast<T>(yyjson_get_real(val));
}

inline void deserialize(bool& out, yyjson_backend& backend)
{
    backend.read_bool(out);
}

template <typename T>
inline void deserialize(std::vector<T>& out, yyjson_val* arr)
{
    out.clear();

    yyjson_arr_iter iter;
    yyjson_arr_iter_init(arr, &iter);

    yyjson_val* val;
    while ((val = yyjson_arr_iter_next(&iter)))
    {
        T& elem = out.emplace_back();
        deserialize(elem, val);
    }
}

template <typename T>
inline void deserialize(std::vector<T>& out, yyjson_backend& backend)
{
    out.clear();

    backend.read_begin_array();
    while (backend.read_has_next())
    {
        T elem;
        deserialize(elem, backend);
        out.push_back(std::move(elem));
    }
    backend.read_end_array();
}

template <typename T>
inline void deserialize(std::optional<T>& out, yyjson_val* val)
{
    if (!val || yyjson_is_null(val))
    {
        out.reset();
        return;
    }

    T& obj = out.emplace();
    deserialize(obj, val);
}

template <typename T>
inline void deserialize(std::optional<T>& out, yyjson_backend& backend)
{
    if (backend.read_is_null())
    {
        backend.read_skip();
        out.reset();
        return;
    }

    T value;
    deserialize(value, backend);
    out = std::move(value);
}

template <typename TString, typename T>
inline void deserialize(std::map<TString, T>& out, yyjson_val* obj)
{
    out.clear();

    yyjson_obj_iter iter;
    yyjson_obj_iter_init(obj, &iter);

    yyjson_val* key;
    while ((key = yyjson_obj_iter_next(&iter)))
    {
        yyjson_val* val = yyjson_obj_iter_get_val(key);

        TString k;
        deserialize(k, key);

        T t;
        deserialize(t, val);

        out.emplace(std::move(k), std::move(t));
    }
}

template <typename TString, typename T>
inline void deserialize(std::map<TString, T>& out, yyjson_backend& backend)
{
    out.clear();

    backend.read_obj_foreach([&](auto&& key)
    {
        T value;
        deserialize(value, backend);
        out.emplace(std::move(key), std::move(value));
    });
}

#endif