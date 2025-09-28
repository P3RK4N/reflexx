#include "comptime/comptest_utils.hpp"

auto _ = StrictNoOpSerializer::serialize(std::string{"hello"});
auto __ = []{ std::string s; StrictNoOpSerializer::deserialize(s, ""); };