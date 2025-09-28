#include "comptime/comptest_utils.hpp"

struct Bad {
    int* ptr;
};

auto _ = StrictNoOpSerializer::serialize(Bad{});