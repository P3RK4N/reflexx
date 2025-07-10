#include "../compile_util.hpp"

struct Bad {
    int& ref;
};

int dummy = 42;
Bad b{dummy};

auto __ = []{ Bad local_b{dummy}; StrictSerializer::deserialize(local_b, ""); };