#include "comptime/comptest_utils.hpp"

struct Address {
    int number;
};

struct Person {
    Address address;
};

auto _ = StrictNoOpSerializer::serialize(Person{});
auto __ = []{ Person p; StrictNoOpSerializer::deserialize(p, ""); };