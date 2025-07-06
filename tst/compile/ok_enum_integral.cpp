#include "compile_util.hpp"

enum class Status { OK, Error };

struct Result {
    Status status;
};

auto _ = StrictSerializer::serialize(Result{});
auto __ = []{ Result r; StrictSerializer::deserialize(r, ""); };