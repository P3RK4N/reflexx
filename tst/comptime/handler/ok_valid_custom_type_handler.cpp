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
        this->key("Key");
        this->serialize_bool(obj.b);
        this->serialize_string(obj.s);
        this->serialize_number(obj.i8);
        this->serialize_number(obj.i16);
        this->serialize_number(obj.i32);
        this->serialize_number(obj.i64);
        this->serialize_number(obj.u8);
        this->serialize_number(obj.u16);
        this->serialize_number(obj.u32);
        this->serialize_number(obj.u64);
        this->serialize_number(obj.f);
        this->serialize_number(obj.d);

        if constexpr (IsReading)
        {
            this->has_next();
            this->is_null();
            this->skip();
        }

        if constexpr (!IsReading)
        {
            this->serialize_null();
        }
    }
};

template class test_struct_handler<StrictSerializer, false>;
template class test_struct_handler<StrictSerializer, true>;