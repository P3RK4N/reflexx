#include "comptime/comptest_utils.hpp"

union BadUnion {
    int a;
    float b;
};

struct Wrapper {
    BadUnion u;
};

auto __ = []{ Wrapper w{}; StrictNoOpSerializer::deserialize(w, ""); };