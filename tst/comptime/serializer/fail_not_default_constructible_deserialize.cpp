#include "../compile_util.hpp"

struct NoDefaultCtor {
    NoDefaultCtor(int) {}
};

auto _ = StrictSerializer::deserialize<NoDefaultCtor>("{}");
