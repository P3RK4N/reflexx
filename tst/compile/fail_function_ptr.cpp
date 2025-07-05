#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

struct Bad {
    void (*fn_ptr)();
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::serialize(Bad{});
