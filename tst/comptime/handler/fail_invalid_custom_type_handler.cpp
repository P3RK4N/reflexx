#include "comptime/comptest_utils.hpp"


struct test_struct
{
    bool b;
    std::string s;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f;
    double d;
};

template <typename TSerializer, bool IsReading>
struct test_struct_handler : public type_handler<TSerializer, IsReading>
{
    void serialize(test_struct& obj)
    {
        this->has_next();
    }
};

using my_serializer = serializer<serializer_settings::Strict(), placeholder_backend, type_handler_list<test_struct_handler, default_handler>>;

auto _ = my_serializer::serialize(test_struct{});
auto __ = []{ Container c; my_serializer::deserialize<test_struct>(""); };