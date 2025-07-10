#include "../compile_util.hpp"

struct Address {
    int number;
};

struct Person {
    Address address;
};

auto _ = StrictSerializer::serialize(Person{});
auto __ = []{ Person p; StrictSerializer::deserialize(p, ""); };