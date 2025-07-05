#include <reflexx/serializer.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;

enum class Status { OK, Error };

struct Result {
    Status status;
};

auto _ = reflexx::serializer<reflexx::serializer_settings::Relaxed(), MyBackend>::serialize(Result{});
