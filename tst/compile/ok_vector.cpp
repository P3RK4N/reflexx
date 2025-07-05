#include <reflexx/serializer.hpp>
#include <vector>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

struct Container {
    std::vector<int> numbers;
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::serialize(Container{});
