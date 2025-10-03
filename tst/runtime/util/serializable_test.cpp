#include "test_utils.hpp"

TEST_CASE("serializable_char_v")
{
    STATIC_REQUIRE      (is_serializable_char_v<char>           );
    STATIC_REQUIRE      (is_serializable_char_v<char8_t>        );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<signed char>    );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<unsigned char>  );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<wchar_t>        );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<char16_t>       );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<char32_t>       );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<int>            );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<float>          );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<double>         );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<long double>    );

    STATIC_REQUIRE_FALSE(is_serializable_char_v<bool>           );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<void>           );
    STATIC_REQUIRE_FALSE(is_serializable_char_v<std::nullptr_t> );
}

TEST_CASE("serializable_enum_v")
{
    enum class MyEnum    { A, B, C };
    enum class EmptyEnum {         };
    enum       OldEnum   { X, Y, Z };

    struct NotEnum {};

    STATIC_REQUIRE(is_serializable_enum_v<MyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<OldEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<EmptyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<const MyEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<volatile OldEnum>);
    STATIC_REQUIRE(is_serializable_enum_v<const volatile MyEnum>);

    STATIC_REQUIRE_FALSE(is_serializable_enum_v<int>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<float>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<double>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<char>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<bool>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<void>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<std::nullptr_t>);
    STATIC_REQUIRE_FALSE(is_serializable_enum_v<NotEnum>);
}

TEST_CASE("serializable_number_v")
{
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<int32_t>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<uint64_t>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<float>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<double>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<unsigned char>);
    STATIC_REQUIRE(reflexx::util::is_serializable_number_v<signed char>);
    
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<long double>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<bool>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<char>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<char8_t>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<void>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_number_v<std::nullptr_t>);
}

TEST_CASE("serializable_string_v")
{
    STATIC_REQUIRE      (is_serializable_string_v<std::string>                       );
    STATIC_REQUIRE      (is_serializable_string_v<std::u8string>                     );
    STATIC_REQUIRE      (is_serializable_string_v<std::string_view>                  );
    STATIC_REQUIRE      (is_serializable_string_v<std::u8string_view>                );

    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::wstring>       );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u16string>     );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u32string>     );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::wstring_view>  );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u16string_view>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::u32string_view>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<char>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<int>                );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<float>              );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<double>             );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<long double>        );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<bool>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<void>               );
    STATIC_REQUIRE_FALSE(reflexx::util::is_serializable_string_v<std::nullptr_t>     );
}

TEST_CASE("serializable_array_v")
{
    STATIC_REQUIRE      (is_serializable_array_v<int[10]>);
    STATIC_REQUIRE_FALSE(is_serializable_array_v<int[]>  );
    STATIC_REQUIRE_FALSE(is_serializable_array_v<int>    );
    STATIC_REQUIRE_FALSE(is_serializable_array_v<int*>   );
}

TEST_CASE("serializable_nullptr_v")
{
    STATIC_REQUIRE      (is_serializable_nullptr_v<std::nullptr_t>);
    STATIC_REQUIRE_FALSE(is_serializable_nullptr_v<int>           );
    STATIC_REQUIRE_FALSE(is_serializable_nullptr_v<int*>          );
    STATIC_REQUIRE_FALSE(is_serializable_nullptr_v<void*>         );
}

TEST_CASE("serializable_bool_v")
{
    STATIC_REQUIRE      (is_serializable_bool_v<bool>  );
    STATIC_REQUIRE_FALSE(is_serializable_bool_v<int>   );
    STATIC_REQUIRE_FALSE(is_serializable_bool_v<double>);
}

TEST_CASE("custom_serializable_class_v")
{
    STATIC_REQUIRE      (is_serializable_class_v<TestStruct>        );

    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::string>       );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::u8string>     );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::string_view>  );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::u8string_view>);
    STATIC_REQUIRE_FALSE(is_serializable_class_v<TestEnum>          );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<int>               );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<float>             );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<double>            );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<bool>              );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<void>              );
    STATIC_REQUIRE_FALSE(is_serializable_class_v<std::nullptr_t>    );
}

TEST_CASE("is_serializable_type trait")
{
    SECTION("Lvalue reference")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<int&>);
    }

    SECTION("Rvalue reference")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<int&&>);
    }

    SECTION("Pointer")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<int*>);
    }

    SECTION("Pointer to member")
    {
        struct S { int m; };

        STATIC_REQUIRE_FALSE(is_serializable_type<int S::*>);
    }

    SECTION("Unbounded array")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<int[]>);
    }

    SECTION("Function type")
    {
        using Fn = void(int);
    
        STATIC_REQUIRE_FALSE(is_serializable_type<Fn>);
    }

    SECTION("Union type")
    {
        union U { int a; float b; };

        STATIC_REQUIRE_FALSE(is_serializable_type<U>);
    }

    SECTION("Void type")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<void>);
    }

    SECTION("Serializable numbers")
    {
        STATIC_REQUIRE      (is_serializable_type<int>           );
        STATIC_REQUIRE      (is_serializable_type<unsigned int>  );
        STATIC_REQUIRE      (is_serializable_type<long>          );
        STATIC_REQUIRE      (is_serializable_type<unsigned long> );
        STATIC_REQUIRE      (is_serializable_type<short>         );
        STATIC_REQUIRE      (is_serializable_type<unsigned short>);
        STATIC_REQUIRE      (is_serializable_type<std::uint8_t>  );
        STATIC_REQUIRE      (is_serializable_type<std::int8_t>   );
        STATIC_REQUIRE      (is_serializable_type<std::uint16_t> );
        STATIC_REQUIRE      (is_serializable_type<std::int16_t>  );
        STATIC_REQUIRE      (is_serializable_type<std::uint32_t> );
        STATIC_REQUIRE      (is_serializable_type<std::int32_t>  );
        STATIC_REQUIRE      (is_serializable_type<std::uint64_t> );
        STATIC_REQUIRE      (is_serializable_type<std::int64_t>  );
        STATIC_REQUIRE      (is_serializable_type<float>         );
        STATIC_REQUIRE      (is_serializable_type<double>        );
        STATIC_REQUIRE      (is_serializable_type<signed char>   );
        STATIC_REQUIRE      (is_serializable_type<unsigned char> );
        STATIC_REQUIRE_FALSE(is_serializable_type<long double>   );
    }

    SECTION("Serializable chars")
    {
        STATIC_REQUIRE_FALSE(is_serializable_type<wchar_t> );
        STATIC_REQUIRE_FALSE(is_serializable_type<char32_t>);
        STATIC_REQUIRE_FALSE(is_serializable_type<char16_t>);
        STATIC_REQUIRE_FALSE(is_serializable_type<char16_t>);

        STATIC_REQUIRE      (is_serializable_type<char>    );
        STATIC_REQUIRE      (is_serializable_type<char8_t> );
    }

    SECTION("Boolean type")
    {
        STATIC_REQUIRE(is_serializable_type<bool>);
    }

    SECTION("Class type")
    {
        struct C {};
    
        STATIC_REQUIRE(is_serializable_type<C>);
    }

    SECTION("Bounded array")
    {
        STATIC_REQUIRE(is_serializable_type<int[5]>);
    }

    SECTION("Enum")
    {
        enum E { A, B };
        enum class EC { A, B };

        STATIC_REQUIRE(is_serializable_type<E>);
        STATIC_REQUIRE(is_serializable_type<EC>);
    }

    SECTION("Nullptr_t")
    {
        STATIC_REQUIRE(is_serializable_type<nullptr_t>);
    }
}
