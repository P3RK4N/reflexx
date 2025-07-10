#include "../compile_util.hpp"

union BadUnion {
    int a;
    float b;
};

struct Wrapper {
    BadUnion u;
};

auto __ = []{ Wrapper w{}; StrictSerializer::deserialize(w, ""); };