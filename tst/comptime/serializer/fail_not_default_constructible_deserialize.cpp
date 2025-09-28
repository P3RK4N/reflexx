#include "comptime/comptest_utils.hpp"

struct NoDefaultCtor {
    NoDefaultCtor(int) {}
};

auto _ = StrictNoOpSerializer::deserialize<NoDefaultCtor>("{}");
