#include <deque>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <optional>
#include <tuple>
#include <variant>
#include <valarray>

#include <catch2/catch_all.hpp>

#include "reflexx/backend.hpp"
#include "reflexx/builtin/backends/ryml_backend.hpp"
#include "reflexx/builtin/backends/yyjson_backend.hpp"
#include "reflexx/serializer.hpp"

#define REFLEXX_DEBUG_TEST_PRINT
using namespace reflexx;

template <reflexx::IsBackendType TBackend>
void builtin_types_test()
{
    using serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), TBackend>;

    SECTION("std::string") {
        std::string original = "hello";
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::string -> {}", *res);
        #endif

        typename serializer::template result<std::string> deserialized = serializer::template deserialize<std::string>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::string (empty)") {
        std::string original = "";
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::string (empty) -> {}", *res);
        #endif

        typename serializer::template result<std::string> deserialized = serializer::template deserialize<std::string>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::string_view") {
        std::string original = "hello";
        auto res = serializer::serialize(original);
        
        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::string_view -> {}", *res);
        #endif

        typename serializer::template result<std::string_view> deserialized = serializer::template deserialize<std::string_view>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::pair") {
        std::pair<int, std::string> original = {42, "world"};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::pair -> {}", *res);
        #endif

        typename serializer::template result<std::pair<int, std::string>> deserialized = serializer::template deserialize<std::pair<int, std::string>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::optional") {
        std::optional<int> original = 1337;
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional -> {}", *res);
        #endif

        typename serializer::template result<std::optional<int>> deserialized = serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::optional (nullopt)") {
        std::optional<int> original = std::nullopt;
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional (nullopt) -> {}", *res);
        #endif

        typename serializer::template result<std::optional<int>> deserialized = serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(!deserialized.get().has_value());
    }

    SECTION("std::shared_ptr") {
        std::shared_ptr<int> original = std::make_shared<int>(1337);
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::shared_ptr -> {}", *res);
        #endif

        typename serializer::template result<std::shared_ptr<int>> deserialized = serializer::template deserialize<std::shared_ptr<int>>(res.get());
        REQUIRE(*original == **deserialized);
    }

    SECTION("std::unique_ptr") {
        std::unique_ptr<int> original = std::make_unique<int>(1337);
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unique_ptr -> {}", *res);
        #endif

        typename serializer::template result<std::unique_ptr<int>> deserialized = serializer::template deserialize<std::unique_ptr<int>>(res.get());
        REQUIRE(*original == **deserialized);
    }

    SECTION("std::tuple") {
        std::tuple<int, std::string, bool> original = {1, "test", true};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::tuple -> {}", *res);
        #endif

        typename serializer::template result<std::tuple<int, std::string, bool>> deserialized = serializer::template deserialize<std::tuple<int, std::string, bool>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::variant") {
        std::variant<int, std::string> original = std::string("variant");
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::variant -> {}", *res);
        #endif

        typename serializer::template result<std::variant<int, std::string>> deserialized = serializer::template deserialize<std::variant<int, std::string>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::vector") {
        std::vector<int> original = {1, 2, 3, 4};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::vector -> {}", *res);
        #endif

        typename serializer::template result<std::vector<int>> deserialized = serializer::template deserialize<std::vector<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::deque") {
        std::deque<int> original = {1, 2, 3, 4};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::deque -> {}", *res);
        #endif

        typename serializer::template result<std::deque<int>> deserialized = serializer::template deserialize<std::deque<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::list") {
        std::list<int> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::list -> {}", *res);
        #endif

        typename serializer::template result<std::list<int>> deserialized = serializer::template deserialize<std::list<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::forward_list") {
        std::forward_list<int> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::forward_list -> {}", *res);
        #endif

        typename serializer::template result<std::forward_list<int>> deserialized = serializer::template deserialize<std::forward_list<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::array") {
        std::array<int, 3> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::array -> {}", *res);
        #endif

        typename serializer::template result<std::array<int, 3>> deserialized = serializer::template deserialize<std::array<int, 3>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::valarray") {
        std::valarray<int> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::valarray -> {}", *res);
        #endif

        typename serializer::template result<std::valarray<int>> deserialized = serializer::template deserialize<std::valarray<int>>(res.get());

        auto valarray_equal = [](const auto& a, const auto& b) {
            return a.size() == b.size() &&
               std::equal(std::begin(a), std::end(a), std::begin(b));
        };

        REQUIRE(valarray_equal(original, *deserialized));
    }

    SECTION("std::map (string)") {
        std::map<std::string, int> original = {{"a", 1}, {"b", 2}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::map (string) -> {}", *res);
        #endif

        typename serializer::template result<std::map<std::string, int>> deserialized = serializer::template deserialize<std::map<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::map (string_view)") {
        std::map<std::string_view, int> original = {{"a", 1}, {"b", 2}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::map (string_view) -> {}", *res);
        #endif

        typename serializer::template result<std::map<std::string_view, int>> deserialized = serializer::template deserialize<std::map<std::string_view, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::map (int)") {
        std::map<int, int> original = {{1, 1}, {2, 2}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::map (int) -> {}", *res);
        #endif

        typename serializer::template result<std::map<int, int>> deserialized = serializer::template deserialize<std::map<int, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::multimap") {
        std::multimap<std::string, int> original = {{"a", 1}, {"b", 2}, {"a", 3}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::multimap -> {}", *res);
        #endif

        typename serializer::template result<std::multimap<std::string, int>> deserialized = serializer::template deserialize<std::multimap<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_map") {
        std::unordered_map<std::string, int> original = {{"a", 1}, {"b", 2}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_map -> {}", *res);
        #endif

        typename serializer::template result<std::unordered_map<std::string, int>> deserialized = serializer::template deserialize<std::unordered_map<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_multimap") {
        std::unordered_multimap<std::string, int> original = {{"a", 1}, {"b", 2}, {"a", 3}};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_multimap -> {}", *res);
        #endif

        typename serializer::template result<std::unordered_multimap<std::string, int>> deserialized = serializer::template deserialize<std::unordered_multimap<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::set") {
        std::set<int> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::set -> {}", *res);
        #endif

        typename serializer::template result<std::set<int>> deserialized = serializer::template deserialize<std::set<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::multiset") {
        std::multiset<int> original = {10, 20, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::multiset -> {}", *res);
        #endif

        typename serializer::template result<std::multiset<int>> deserialized = serializer::template deserialize<std::multiset<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_set") {
        std::unordered_set<int> original = {10, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_set -> {}", *res);
        #endif

        typename serializer::template result<std::unordered_set<int>> deserialized = serializer::template deserialize<std::unordered_set<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_multiset") {
        std::unordered_multiset<int> original = {10, 20, 20, 30};
        auto res = serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_multiset -> {}", *res);
        #endif

        typename serializer::template result<std::unordered_multiset<int>> deserialized = serializer::template deserialize<std::unordered_multiset<int>>(res.get());
        REQUIRE(original == *deserialized);
    }
}

TEST_CASE("Builtin types using yyjson_backend")
{
    builtin_types_test<reflexx::backends::yyjson_backend>();
}

TEST_CASE("Builtin types using ryml_backend")
{
    builtin_types_test<reflexx::backends::ryml_backend>();
}


// TODO: Make these tests better
//  - Currently it only checks for root leaf value behaviour
//  - Expand to see behaviour inside array and object
//  - expand to see how everything behaves when empty or null
//  - expand to see how everything behaves as optional or ptr


/**
def testing shit here:
    for (as value, as optional value, as shared_ptr, as unique_ptr)
        for (as leaf root, as array element, as object field)
            for (type: types)
                SECTION(...)

For contaners, call it multiple times: empty, 1 and few (associative with string and non string)
For nullable, nullptrable: non null and null

 */