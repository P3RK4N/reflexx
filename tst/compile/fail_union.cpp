#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

union BadUnion {
    int a;
    float b;
};

struct Wrapper {
    BadUnion u;
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::serialize(Wrapper{});
