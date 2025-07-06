#include "compile_util.hpp"

struct Bad {
    int* ptr;
};

auto _ = StrictSerializer::serialize(Bad{});
auto __ = []{ Bad b; StrictSerializer::deserialize(b, ""); };