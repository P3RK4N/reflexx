#ifndef REFLEXX_TEST_UTILS_HPP
#define REFLEXX_TEST_UTILS_HPP

#include "reflexx/declare.hpp"
#include "reflexx/policies.hpp"
#include "reflexx/serializer.hpp"

#include "reflexx/builtin/backends/placeholder_backend.hpp"

#include "reflexx/builtin/handlers/default_handler.hpp"
#include "reflexx/builtin/handlers/std_handler.hpp"
#include "reflexx/builtin/handlers/ptr_graph_handler.hpp"
#include "reflexx/serializer_settings.hpp"

#include "reflexx/util/std_util.hpp"
#include "reflexx/util/function_traits.hpp"
#include "reflexx/util/serializable_enum.hpp"
#include "reflexx/util/serializable_string.hpp"
#include "reflexx/util/serializable_class.hpp"

#include <tuple>
#include <cstdint>
#include <string>
#include <string_view>

#include <type_traits>

using namespace ::reflexx;
using namespace ::reflexx::util;
using namespace ::reflexx::policies;
using namespace ::reflexx::backends;

enum class TestEnum : char
{
    ValueA,
    ValueB,
    ValueC
};

struct TestStruct
{
    int a = 11;
    std::string b = "049";
    bool c = true;

    bool operator==(const TestStruct&) const = default;
};

struct Empty
{
    bool operator==(const Empty&) const = default;
};

struct TestTrivialStruct
{
    struct SubStruct
    {
        float x = 3.14f;
        double y = 2.71828;

        bool operator==(const SubStruct&) const = default;
    };

    // Boolean
    bool bool_val = true;

    // Character types
    char        char_val   = 'A';
    signed char schar_val  = -42;
    unsigned char uchar_val = 200;
    // char8_t char8_val  = u8'N';

    // Integer types with fixed width
    int16_t  short_val  = -123;           // replaces 'short'
    uint16_t ushort_val = 456u;           // replaces 'unsigned short'
    int32_t  int_val    = -789;           // replaces 'int'
    uint32_t uint_val   = 1234u;          // replaces 'unsigned int'
    int32_t  long_val   = -98765L;        // typically 'long' is 32-bit on most platforms
    uint32_t ulong_val  = 54321UL;        // unsigned version
    int64_t  ullong_val = 987654321LL;    // replaces 'unsigned long long'
    uint64_t  llong_val = 987654321LL;    // replaces 'long long'

    // Floating-point types
    float       float_val  = 1.5f;
    double      double_val = 2.25;

    // Nested struct
    SubStruct sub {};

    bool operator==(const TestTrivialStruct&) const = default;
};

template <typename T>
struct TestTemplatedStruct
{
    TestTemplatedStruct(const T& val) : specialMember(val) {}
    TestTemplatedStruct() {}
    
    TestStruct member1;
    TestTrivialStruct member2;
    T specialMember;

    bool operator==(const TestTemplatedStruct&) const = default;
};

using values_t = std::tuple
<
    char,
    char8_t,
    bool,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    float,
    double,
    std::nullptr_t,
    TestEnum,
    std::string,
    std::string_view
>;

constexpr serializer_settings StrictSettings               = serializer_settings::Strict();
constexpr serializer_settings RelaxedSettings              = serializer_settings::Relaxed();

constexpr serializer_settings serializer_settings_matrix[] = 
{
    StrictSettings, 
    RelaxedSettings 
};

using StrictNoOpSerializer = serializer<StrictSettings, placeholder_backend>;
using RelaxedNoOpSerializer = serializer<RelaxedSettings, placeholder_backend>;

#endif