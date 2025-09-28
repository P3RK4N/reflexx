#include "comptime/comptest_utils.hpp"

struct Person {
    std::string name;
    int age;
};

auto _ = StrictNoOpSerializer::serialize(Person{"Ana", 23});
auto __ = []{ Person p{"", 0}; StrictNoOpSerializer::deserialize(p, ""); };