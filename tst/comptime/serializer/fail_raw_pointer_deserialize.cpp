#include "../compile_util.hpp"

struct Bad {
    int* ptr;
};

auto __ = []{ Bad b; StrictSerializer::deserialize(b, ""); };