#include "../compile_util.hpp"

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
struct test_struct_handler : public reflexx::type_handler<TSerializer, IsReading>
{
    void serialize(test_struct& obj)
    {
        this->has_next();
    }
};

template class test_struct_handler<StrictSerializer, false>;
template class test_struct_handler<StrictSerializer, true>;