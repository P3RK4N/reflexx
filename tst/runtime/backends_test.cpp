#include <catch2/catch_all.hpp>
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

#include "reflexx/backend.hpp"
#include "reflexx/serializer.hpp"
#include "reflexx/backends/yyjson_backend.hpp"

// #define REFLEXX_DEBUG_TEST_PRINT

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

        std::string deserialized = Serializer::template deserialize<std::string>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::pair") {
        std::pair<int, std::string> original = {42, "world"};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::pair -> {}", *res);
        #endif

        std::pair<int, std::string> deserialized = Serializer::template deserialize<std::pair<int, std::string>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::optional") {
        std::optional<int> original = 1337;
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional -> {}", *res);
        #endif

        std::optional<int> deserialized = Serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::optional (nullopt)") {
        std::optional<int> original = std::nullopt;
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::optional (nullopt) -> {}", *res);
        #endif

        std::optional<int> deserialized = Serializer::template deserialize<std::optional<int>>(res.get());
        REQUIRE(!deserialized.has_value());
    }

    SECTION("std::tuple") {
        std::tuple<int, std::string, bool> original = {1, "test", true};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::tuple -> {}", *res);
        #endif

        std::tuple<int, std::string, bool> deserialized = Serializer::template deserialize<std::tuple<int, std::string, bool>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::variant") {
        std::variant<int, std::string> original = std::string("variant");
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::variant -> {}", *res);
        #endif

        std::variant<int, std::string> deserialized = Serializer::template deserialize<std::variant<int, std::string>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::vector") {
        std::vector<int> original = {1, 2, 3, 4};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::vector -> {}", *res);
        #endif

        std::vector<int> deserialized = Serializer::template deserialize<std::vector<int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::list") {
        std::list<int> original = {10, 20, 30};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::list -> {}", *res);
        #endif

        std::list<int> deserialized = Serializer::template deserialize<std::list<int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::set") {
        std::set<int> original = {3, 1, 4};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::set -> {}", *res);
        #endif

        std::set<int> deserialized = Serializer::template deserialize<std::set<int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::unordered_set") {
        std::unordered_set<int> original = {3, 1, 4};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_set -> {}", *res);
        #endif

        std::unordered_set<int> deserialized = Serializer::template deserialize<std::unordered_set<int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::map") {
        std::map<std::string, int> original = {{"a", 1}, {"b", 2}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::map -> {}", *res);
        #endif

        std::map<std::string, int> deserialized = Serializer::template deserialize<std::map<std::string, int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::unordered_map") {
        std::unordered_map<std::string, int> original = {{"x", 10}, {"y", 20}};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::unordered_map -> {}", *res);
        #endif

        std::unordered_map<std::string, int> deserialized = Serializer::template deserialize<std::unordered_map<std::string, int>>(res.get());
        REQUIRE(original == deserialized);
    }

    SECTION("std::valarray") {
        std::valarray<int> original = {5, 6, 7, 8};
        auto res = Serializer::serialize(original);

        #ifdef REFLEXX_DEBUG_TEST_PRINT
            std::println("std::valarray -> {}", *res);
        #endif

        std::valarray<int> deserialized = Serializer::template deserialize<std::valarray<int>>(res.get());
        REQUIRE(original.size() == deserialized.size());
        for (std::size_t i = 0; i < original.size(); ++i) {
            REQUIRE(original[i] == deserialized[i]);
        }
    }
}

TEST_CASE("Builting types using yyjson_backend")
{
    builtin_types_test<reflexx::backends::YyjsonBackend>();
}