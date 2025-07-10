#include "../compile_util.hpp"

struct Bad {
    int* ptr;
};

auto _ = StrictSerializer::serialize(Bad{});