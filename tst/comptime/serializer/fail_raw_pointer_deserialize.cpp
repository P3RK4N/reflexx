#include "comptime/comptest_utils.hpp"

struct Bad {
    int* ptr;
};

auto __ = []{ Bad b; StrictNoOpSerializer::deserialize(b, ""); };