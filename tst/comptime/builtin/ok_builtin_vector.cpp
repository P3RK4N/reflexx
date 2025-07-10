#include "../compile_util.hpp"

auto _ = StrictSerializer::serialize(std::vector<int>{1,2,3});
auto __ = []{ std::vector<int> v; StrictSerializer::deserialize(v, ""); };