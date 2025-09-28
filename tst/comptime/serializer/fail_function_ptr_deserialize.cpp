#include "comptime/comptest_utils.hpp"

struct Bad {
    void (*fn_ptr)();
};

auto __ = []{ Bad b; StrictNoOpSerializer::deserialize(b, ""); };