#include "../compile_util.hpp"

struct Allowed {
    int* ignored;
};

auto _ = RelaxedSerializer::serialize(Allowed{});
auto __ = []{ Allowed a; RelaxedSerializer::deserialize(a, ""); };