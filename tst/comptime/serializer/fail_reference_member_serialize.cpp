#include "comptime/comptest_utils.hpp"

struct Bad {
    int& ref;
};

int dummy = 42;
Bad b{dummy};

auto _ = StrictNoOpSerializer::serialize(b);