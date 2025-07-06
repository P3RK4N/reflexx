#include "compile_util.hpp"

union BadUnion {
    int a;
    float b;
};

struct Wrapper {
    BadUnion u;
};

auto _ = StrictSerializer::serialize(Wrapper{});
auto __ = []{ Wrapper w{}; StrictSerializer::deserialize(w, ""); };