#include "compile_util.hpp"

struct Container {
    std::vector<int> numbers;
};

auto _ = StrictSerializer::serialize(Container{});
auto __ = []{ Container c; StrictSerializer::deserialize(c, ""); };