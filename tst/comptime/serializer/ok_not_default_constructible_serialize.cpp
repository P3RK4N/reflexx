#include "comptime/comptest_utils.hpp"

struct NoDefaultCtor {
    NoDefaultCtor(int) {}
};

auto _ = StrictNoOpSerializer::serialize<NoDefaultCtor>(NoDefaultCtor { 1 });
