#include "comptime/comptest_utils.hpp"

enum class Status { OK, Error };

struct Result {
    Status status;
};

auto _ = RelaxedNoOpSerializer::serialize(Result{});
auto __ = []{ Result r; RelaxedNoOpSerializer::deserialize(r, ""); };