#include "../compile_util.hpp"
struct S
{
    int i;
};

template <typename TSerializer, bool IsReading>
struct handler_S : public reflexx::type_handler<TSerializer, IsReading>
{
    void serialize(S& obj) const
    {
        this->key("i");
        this->serialize_number(obj.i);
    }
};

using serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend, reflexx::type_handler_list<handler_S>>;

auto _ = serializer::serialize(S{});