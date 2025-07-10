#include "../compile_util.hpp"

struct Bad {
    int& ref;
};

int dummy = 42;
Bad b{dummy};

auto _ = StrictSerializer::serialize(b);