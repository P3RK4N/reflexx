#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

struct NoDefaultCtor {
    NoDefaultCtor(int) {}
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::deserialize<NoDefaultCtor>("{}");
