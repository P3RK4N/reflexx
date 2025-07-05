#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

struct Bad {
    int& ref;
};

int dummy = 42;
Bad b{dummy};

reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>::deserialize<Bad>(b, "{}");