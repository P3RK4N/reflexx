#include "comptime/comptest_utils.hpp"

struct Bad {
    void (*fn_ptr)();
};

auto _ = StrictNoOpSerializer::serialize(Bad{});