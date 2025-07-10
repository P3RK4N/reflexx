#include "../compile_util.hpp"

using serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend, reflexx::type_handler_list<>>;

struct S
{
    int i;
};

auto _ = serializer::serialize(S{});