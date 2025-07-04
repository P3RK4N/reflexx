#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

#include "serializer_settings.hpp"
#include "serializer.hpp"
#include "backends/placeholder.hpp"
#include "backends/yyjson_backend.hpp"

template <typename... Ts>
void pprint(const Ts&... items)
{
    (std::print("{} ", items), ...);
    std::println();
}

constexpr auto settings = reflexx::serializer_settings::Relaxed();

// Runtime only
using s = reflexx::serializer<settings, reflexx::backends::YyjsonBackend>;
// Constexpr friendly
using cxs = reflexx::serializer<settings, reflexx::backends::placeholder_backend>;

TEST_CASE( "Playground", "[MISC]" ) {

    struct aa
    {
        int a = 2;
        const int b = 3;
    };

    // pprint(s::serialize(aa{}).get());
    s::deserialize(aa{});

}
