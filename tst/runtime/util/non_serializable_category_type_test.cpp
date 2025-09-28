#include "test_utils.hpp"

TEST_CASE("is_non_serializable_category_type_v trait")
{

    SECTION("Lvalue reference")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int&>);
    }

    SECTION("Rvalue reference")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int&&>);
    }

    SECTION("Pointer")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int*>);
    }

    SECTION("Pointer to member")
    {
        struct S { int m; };

        STATIC_REQUIRE(is_non_serializable_category_type_v<int S::*>);
    }

    SECTION("Unbounded array")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<int[]>);
    }

    SECTION("Function type")
    {
        using Fn = void(int);
    
        STATIC_REQUIRE(is_non_serializable_category_type_v<Fn>);
    }

    SECTION("Union type")
    {
        union U { int a; float b; };

        STATIC_REQUIRE(is_non_serializable_category_type_v<U>);
    }

    SECTION("Void type")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<void>);
    }

    SECTION("Serializable numbers")
    {
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<int>           );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<unsigned int>  );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<long>          );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<unsigned long> );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<short>         );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<unsigned short>);
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::uint8_t>  );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::int8_t>   );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::uint16_t> );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::int16_t>  );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::uint32_t> );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::int32_t>  );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::uint64_t> );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<std::int64_t>  );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<float>         );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<double>        );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<signed char>   );
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<unsigned char> );
        
        // TODO: Check this out
        STATIC_REQUIRE      (is_non_serializable_category_type_v<long double>   );
    }

    SECTION("Serializable chars")
    {
        STATIC_REQUIRE(is_non_serializable_category_type_v<wchar_t>);
        STATIC_REQUIRE(is_non_serializable_category_type_v<char32_t>);
        STATIC_REQUIRE(is_non_serializable_category_type_v<char16_t>);
        STATIC_REQUIRE(is_non_serializable_category_type_v<char16_t>);

        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<char>);
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<char8_t>);
    }

    SECTION("Boolean type")
    {
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<bool>);
    }

    SECTION("Class type")
    {
        struct C {};
    
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<C>);
    }

    SECTION("Bounded array")
    {
        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<int[5]>);
    }

    SECTION("Enum")
    {
        enum E { A, B };

        STATIC_REQUIRE_FALSE(is_non_serializable_category_type_v<E>);
    }
}
