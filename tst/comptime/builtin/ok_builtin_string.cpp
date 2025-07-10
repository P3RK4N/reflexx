#include "../compile_util.hpp"

auto _ = StrictSerializer::serialize(std::string{"hello"});
auto __ = []{ std::string s; StrictSerializer::deserialize(s, ""); };