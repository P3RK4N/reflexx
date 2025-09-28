#include "comptime/comptest_utils.hpp"


struct Container {
    std::vector<int> numbers;
};

auto _ = StrictNoOpSerializer::serialize(Container{});
auto __ = []{ Container c; StrictNoOpSerializer::deserialize(c, ""); };