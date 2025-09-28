#include "comptime/comptest_utils.hpp"


auto _ = StrictNoOpSerializer::serialize(std::vector<int>{1,2,3});
auto __ = []{ std::vector<int> v; StrictNoOpSerializer::deserialize(v, ""); };