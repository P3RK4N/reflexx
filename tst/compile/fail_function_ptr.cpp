#include "compile_util.hpp"

struct Bad {
    void (*fn_ptr)();
};

auto _ = StrictSerializer::serialize(Bad{});
auto __ = []{ Bad b; StrictSerializer::deserialize(b, ""); };