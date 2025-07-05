#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

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

TEST_CASE( "Playground", "[MISC]" ) {

    struct aa
    {
        int& a;
        const int b = 3;
    };
    int a = 2;
    aa deser { a };
    pprint(s::serialize(aa{a}).get());
    s::deserialize(deser, s::serialize(aa{a}).get());

}
