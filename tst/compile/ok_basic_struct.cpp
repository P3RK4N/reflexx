#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

struct Person {
    std::string name;
    int age;
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::serialize(Person{"Ana", 23});
