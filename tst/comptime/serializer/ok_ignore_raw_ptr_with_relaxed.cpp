#include "comptime/comptest_utils.hpp"

struct Allowed {
    int* ignored;
};

auto _ = RelaxedNoOpSerializer::serialize(Allowed{});
auto __ = []{ Allowed a; RelaxedNoOpSerializer::deserialize(a, ""); };