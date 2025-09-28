#include "comptime/comptest_utils.hpp"

struct S
{
    int i;
};

template <typename TSerializer, bool IsReading>
struct handler_S : public type_handler<TSerializer, IsReading>
{
    void serialize(S& obj) const
    {
        this->key("i");
        this->serialize_number(obj.i);
    }
};

using my_serializer = serializer<StrictSettings, placeholder_backend, type_handler_list<handler_S>>;

auto _ = my_serializer::serialize(S{});