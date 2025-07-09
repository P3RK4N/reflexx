#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

#include "reflexx/declare.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/serializer.hpp"
#include "reflexx/backends/placeholder.hpp"
#include "reflexx/backends/yyjson_backend.hpp"

template <typename... Ts>
void pprint(const Ts&... items)
{
    (std::print("{} ", items), ...);
    std::println();
}

constexpr auto settings = reflexx::serializer_settings::Strict();

// Runtime only
using s = reflexx::serializer<settings, reflexx::backends::YyjsonBackend>;
// Constexpr friendly
using cxs = reflexx::serializer<settings, reflexx::backends::placeholder_backend>;

template <typename T>
struct serialize_impl {
    template <typename Backend>
    static void apply(Backend&, const T&) = delete;
};


template <>
struct serialize_impl<std::string> {
    template <typename Backend>
    static void apply(Backend& backend, const std::string& value) {
        backend.write_string(value); // adjust to your backend's API
    }
};

template <typename T>
static constexpr bool has_builtin_handler_v = requires (T obj, int& ctx) {
    serialize_impl<T>::apply(ctx, obj);
};

struct Struktura {
    std::string ime;
    int godine;
    std::vector<std::string> hobiji;
};

#include "reflexx/handler_list.hpp"

template <typename TSerializer, bool IsReading>
struct handler_string : reflexx::custom_type_handler<TSerializer, IsReading>
{
    void serialize(std::string& str)
    {
    }
};

template <typename T, bool b>
class ddefault {};

TEST_CASE( "Playground", "[MISC]" )
{
    using dt = reflexx::handler_list<handler_string, reflexx::default_type_handler>;
    using s = reflexx::serializer<reflexx::serializer_settings::Strict(), reflexx::backends::placeholder_backend>;

    using string_handler = dt::get_first_t<s, true, std::string>;
    using stringview_handler = dt::get_first_t<s, true, std::string_view>;

    stringview_handler hndlr;
    std::string_view sv = "Hello";
    // hndlr.serialize(sv);
    // using dtt = dt::append<std::string, handler_string>;
    // std::println("{}", reflexx::HasHandlerFor<handler_string<s, true>, std::string>);

    // std::println("{}", std::meta::display_string_of(std::meta::dealias(^^typename dt::template get_first_or_default_tt<std::string, s, true, ddefault>)));
    // pprint(std::meta::display_string_of(std::meta::dealias(^^typename dispatcher::get_first_or_default_t<float>)));
    // pprint(std::meta::display_string_of(std::meta::dealias(^^typename dispatcher::get_first_or_default_t<int>)));

}


