#include "comptime/comptest_utils.hpp"

enum class Status { OK, Error };

struct Result {
    Status status;
};

auto _ = StrictNoOpSerializer::serialize(Result{});
auto __ = []{ Result r; StrictNoOpSerializer::deserialize(r, ""); };