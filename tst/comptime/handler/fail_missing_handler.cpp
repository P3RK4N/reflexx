#include "comptime/comptest_utils.hpp"

using my_serializer = serializer<StrictSettings, placeholder_backend, type_handler_list<>>;

auto _ = my_serializer::serialize(Empty{});