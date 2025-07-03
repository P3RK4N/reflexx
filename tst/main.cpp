#include <cstring>
#include <experimental/meta>
#include <print>

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

int main()
{
    auto res = s::serialize(std::vector<int>{ 1, 2, 3, 4, 5 });
    pprint(res.get());
}