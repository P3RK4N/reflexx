#include "test_utils.hpp"

TEST_CASE("enumerate produces correct sequences")
{
    STATIC_REQUIRE(enumerate<0>.size() == 0);
    STATIC_REQUIRE(enumerate<10>.size() == 10);

    template for (constexpr std::size_t I : { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 })
    {
        STATIC_REQUIRE(enumerate<10>[I] == I);
    }
}