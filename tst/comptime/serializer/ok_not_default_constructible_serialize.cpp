#include "../compile_util.hpp"

struct NoDefaultCtor {
    NoDefaultCtor(int) {}
};

auto _ = StrictSerializer::serialize<NoDefaultCtor>(NoDefaultCtor { 1 });
