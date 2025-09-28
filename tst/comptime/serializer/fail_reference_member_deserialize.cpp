#include "comptime/comptest_utils.hpp"

struct Bad {
    int& ref;
};

int dummy = 42;
Bad b{dummy};

auto __ = []{ Bad local_b{dummy}; StrictNoOpSerializer::deserialize(local_b, ""); };