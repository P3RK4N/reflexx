#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

#include "reflexx/declare.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/serializer.hpp"
#include "reflexx/backends/placeholder.hpp"
#include "reflexx/backends/yyjson_backend.hpp"
#include "reflexx/type_handler_list.hpp"

template <typename... Ts>
void pyprint(const Ts&... items)
{
    (std::print("{} ", items), ...);
    std::println();
}

constexpr auto settings = reflexx::serializer_settings::Strict();
using s = reflexx::serializer<settings, reflexx::backends::YyjsonBackend>;



TEST_CASE( "Playground", "[MISC]" )
{
    pyprint("Ante", "Ivan", 2, 2.3, *s::serialize(std::vector<int>{ 1, 4, 2 }));
}