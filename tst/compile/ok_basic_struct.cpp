#include "compile_util.hpp"

struct Person {
    std::string name;
    int age;
};

auto _ = StrictSerializer::serialize(Person{"Ana", 23});
auto __ = []{ Person p{"", 0}; StrictSerializer::deserialize(p, ""); };