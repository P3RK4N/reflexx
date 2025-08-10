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
#include "reflexx/serializer.hpp"
#include "reflexx/backends/yyjson_backend.hpp"

// #define REFLEXX_DEBUG_TEST_PRINT
using namespace reflexx;

template <reflexx::IsBackendType TBackend>
void builtin_types_test()
{
    using Serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), TBackend>;

    SECTION("std::string") {
        std::string original = "hello";
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::string -> {}", *res);
        #endif

        result_holder<std::string> deserialized = Serializer::template deserialize<std::string>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::pair") {
        std::pair<int, std::string> original = {42, "world"};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::pair -> {}", *res);
        #endif

        result_holder<std::pair<int, std::string>> deserialized = Serializer::template deserialize<std::pair<int, std::string>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::optional") {
        std::optional<int> original = 1337;
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional -> {}", *res);
        #endif

        result_holder<std::optional<int>> deserialized = Serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::optional (nullopt)") {
        std::optional<int> original = std::nullopt;
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional (nullopt) -> {}", *res);
        #endif

        result_holder<std::optional<int>> deserialized = Serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(!deserialized.get().has_value());
    }

    SECTION("std::shared_ptr") {
        std::shared_ptr<int> original = std::make_shared<int>(1337);
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::shared_ptr -> {}", *res);
        #endif

        result_holder<std::shared_ptr<int>> deserialized = Serializer::template deserialize<std::shared_ptr<int>>(res.get());
        REQUIRE(*original == **deserialized);
    }

    SECTION("std::unique_ptr") {
        std::unique_ptr<int> original = std::make_unique<int>(1337);
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unique_ptr -> {}", *res);
        #endif

        result_holder<std::unique_ptr<int>> deserialized = Serializer::template deserialize<std::unique_ptr<int>>(res.get());
        REQUIRE(*original == **deserialized);
    }

    SECTION("std::tuple") {
        std::tuple<int, std::string, bool> original = {1, "test", true};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::tuple -> {}", *res);
        #endif

        result_holder<std::tuple<int, std::string, bool>> deserialized = Serializer::template deserialize<std::tuple<int, std::string, bool>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::variant") {
        std::variant<int, std::string> original = std::string("variant");
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::variant -> {}", *res);
        #endif

        result_holder<std::variant<int, std::string>> deserialized = Serializer::template deserialize<std::variant<int, std::string>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::vector") {
        std::vector<int> original = {1, 2, 3, 4};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::vector -> {}", *res);
        #endif

        result_holder<std::vector<int>> deserialized = Serializer::template deserialize<std::vector<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::deque") {
        std::deque<int> original = {1, 2, 3, 4};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::deque -> {}", *res);
        #endif

        result_holder<std::deque<int>> deserialized = Serializer::template deserialize<std::deque<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::list") {
        std::list<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::list -> {}", *res);
        #endif

        result_holder<std::list<int>> deserialized = Serializer::template deserialize<std::list<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::forward_list") {
        std::forward_list<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::forward_list -> {}", *res);
        #endif

        result_holder<std::forward_list<int>> deserialized = Serializer::template deserialize<std::forward_list<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::array") {
        std::array<int, 3> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::array -> {}", *res);
        #endif

        result_holder<std::array<int, 3>> deserialized = Serializer::template deserialize<std::array<int, 3>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::valarray") {
        std::valarray<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::valarray -> {}", *res);
        #endif

        result_holder<std::valarray<int>> deserialized = Serializer::template deserialize<std::valarray<int>>(res.get());

        auto valarray_equal = [](const auto& a, const auto& b) {
            return a.size() == b.size() &&
               std::equal(std::begin(a), std::end(a), std::begin(b));
        };

        REQUIRE(valarray_equal(original, *deserialized));
    }

    SECTION("std::map") {
        std::map<std::string, int> original = {{"a", 1}, {"b", 2}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::map -> {}", *res);
        #endif

        result_holder<std::map<std::string, int>> deserialized = Serializer::template deserialize<std::map<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::multimap") {
        std::multimap<std::string, int> original = {{"a", 1}, {"b", 2}, {"a", 3}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::multimap -> {}", *res);
        #endif

        result_holder<std::multimap<std::string, int>> deserialized = Serializer::template deserialize<std::multimap<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_map") {
        std::unordered_map<std::string, int> original = {{"a", 1}, {"b", 2}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_map -> {}", *res);
        #endif

        result_holder<std::unordered_map<std::string, int>> deserialized = Serializer::template deserialize<std::unordered_map<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_multimap") {
        std::unordered_multimap<std::string, int> original = {{"a", 1}, {"b", 2}, {"a", 3}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_multimap -> {}", *res);
        #endif

        result_holder<std::unordered_multimap<std::string, int>> deserialized = Serializer::template deserialize<std::unordered_multimap<std::string, int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::set") {
        std::set<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::set -> {}", *res);
        #endif

        result_holder<std::set<int>> deserialized = Serializer::template deserialize<std::set<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::multiset") {
        std::multiset<int> original = {10, 20, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::multiset -> {}", *res);
        #endif

        result_holder<std::multiset<int>> deserialized = Serializer::template deserialize<std::multiset<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_set") {
        std::unordered_set<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_set -> {}", *res);
        #endif

        result_holder<std::unordered_set<int>> deserialized = Serializer::template deserialize<std::unordered_set<int>>(res.get());
        REQUIRE(original == *deserialized);
    }

    SECTION("std::unordered_multiset") {
        std::unordered_multiset<int> original = {10, 20, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_multiset -> {}", *res);
        #endif

        result_holder<std::unordered_multiset<int>> deserialized = Serializer::template deserialize<std::unordered_multiset<int>>(res.get());
        REQUIRE(original == *deserialized);
    }
}

TEST_CASE("Builtin types using yyjson_backend")
{
    builtin_types_test<reflexx::backends::yyjson_backend>();
}