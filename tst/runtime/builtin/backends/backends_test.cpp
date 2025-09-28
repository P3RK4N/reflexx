#include "test_utils.hpp"

TEMPLATE_LIST_TEST_CASE("serializable number", "", serializers_list)
{
    using T = TestType;

    roundtrip_check<T>(static_cast<signed char>(-42));
    roundtrip_check<T>(static_cast<unsigned char>(42));
    roundtrip_check<T>(static_cast<short>(-1234));
    roundtrip_check<T>(static_cast<unsigned short>(1234));
    roundtrip_check<T>(static_cast<int>(-123456));
    roundtrip_check<T>(static_cast<unsigned int>(123456u));
    roundtrip_check<T>(static_cast<long>(-123456789l));
    roundtrip_check<T>(static_cast<unsigned long>(123456789ul));

    roundtrip_check<T>(int8_t(-100));
    roundtrip_check<T>(uint8_t(200));
    roundtrip_check<T>(int16_t(-30000));
    roundtrip_check<T>(uint16_t(60000));
    roundtrip_check<T>(int32_t(-2000000000));
    roundtrip_check<T>(uint32_t(4000000000u));
    roundtrip_check<T>(int64_t(-9000000000000000000ll));
    roundtrip_check<T>(uint64_t(18000000000000000000ull));

    roundtrip_check<T>(static_cast<float>(3.675f));
    roundtrip_check<T>(static_cast<double>(2.125));
}

TEMPLATE_LIST_TEST_CASE("serializable char", "", serializers_list)
{
    using T = TestType;

    roundtrip_check<T>(static_cast<char>('x'));
    roundtrip_check<T>(static_cast<char8_t>(u8'a'));
}

TEMPLATE_LIST_TEST_CASE("serializable string", "", serializers_list)
{
    using T = TestType;

    std::string       s1 = "hello Perkovic";
    std::u8string     s2 = u8"hello Perković world ñüø";
    std::string_view  sv1 = s1;
    std::u8string_view sv2 = s2;

    roundtrip_check<T>(s1);
    roundtrip_check<T>(s2);
    roundtrip_check<T>(sv1);
    roundtrip_check<T>(sv2);
}

TEMPLATE_LIST_TEST_CASE("serializable enum", "", serializers_list)
{
    using T = TestType;

    roundtrip_check<T>(TestEnum::ValueA);
    roundtrip_check<T>(TestEnum::ValueB);
    roundtrip_check<T>(TestEnum::ValueC);
}

TEMPLATE_LIST_TEST_CASE("serializable nullptr", "", serializers_list)
{
    using T = TestType;

    roundtrip_check<T>(nullptr);
}

TEMPLATE_LIST_TEST_CASE("serializable bool", "", serializers_list)
{
    using T = TestType;

    roundtrip_check<T>(true);
    roundtrip_check<T>(false);
}

TEMPLATE_LIST_TEST_CASE("serializable object", "", serializers_list)
{
    using T = TestType;

    TestStruct value{};
    roundtrip_check<T>(value);

    roundtrip_check<T>(Empty{});
}

TEMPLATE_LIST_TEST_CASE("serializable arrays", "", serializers_list)
{
    using T = TestType;

    int arr[5] = {1, 2, 3, 4, 5};
    int val[5] { 0 };

    auto serialized = T::serialize(arr);   
    auto deserialized = T::deserialize(val, *serialized);
    REQUIRE(std::memcmp(*deserialized, arr, sizeof(arr)) == 0);
}

TEMPLATE_LIST_TEST_CASE("serializable nested combinations", "", serializers_list)
{
    using T = TestType;

    SECTION("array of arrays")
    {
        int arr_arr[2][3] =
        {
            {1, 2, 3},
            {4, 5, 6}
        };
        int out_arr_arr[2][3] = {};
        auto ser = T::serialize(arr_arr);
        auto deser = T::deserialize(out_arr_arr, *ser);
        REQUIRE(std::memcmp(*deser, arr_arr, sizeof(arr_arr)) == 0);
    }

    SECTION("array of objects")
    {
        TestTemplatedStruct<int> arr_obj[2] =
        {
            TestTemplatedStruct<int>(42),
            TestTemplatedStruct<int>(7)
        };
        TestTemplatedStruct<int> out_arr_obj[2];

        auto ser = T::serialize(arr_obj);
        auto deser = T::deserialize(out_arr_obj, *ser);
        REQUIRE(out_arr_obj[0] == arr_obj[0]);
        REQUIRE(out_arr_obj[1] == arr_obj[1]);
    }

    SECTION("object with array member")
    {
        struct ArrHolder
        {
            int data[3];
            bool operator==(const ArrHolder& o) const
            {
                return std::memcmp(data, o.data, sizeof(data)) == 0;
            }
        };

        TestTemplatedStruct<ArrHolder> obj(ArrHolder{{ 10, 20, 30 }});
        roundtrip_check<T>(obj);
    }

    SECTION("object with object member")
    {
        struct Inner
        {
            int x;
            Empty e;
            bool operator==(const Inner&) const = default;
        };
        struct Outer
        {
            Inner inner;
            Empty e;
            bool operator==(const Outer&) const = default;
        };

        TestTemplatedStruct<int> inner { 42 };
        TestTemplatedStruct<TestTemplatedStruct<int>> outer { inner };

        roundtrip_check<T>(outer);
    }
}