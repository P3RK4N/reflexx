#include "compile_util.hpp"

enum class Status { OK, Error };

struct Result {
    Status status;
};

auto _ = RelaxedSerializer::serialize(Result{});
auto __ = []{ Result r; RelaxedSerializer::deserialize(r, ""); };