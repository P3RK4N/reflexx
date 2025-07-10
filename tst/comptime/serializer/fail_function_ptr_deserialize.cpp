#include "../compile_util.hpp"

struct Bad {
    void (*fn_ptr)();
};

auto __ = []{ Bad b; StrictSerializer::deserialize(b, ""); };