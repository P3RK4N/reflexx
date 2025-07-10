#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/serializer.hpp"
#include "reflexx/backends/placeholder.hpp"
#include "reflexx/backends/yyjson_backend.hpp"

template <typename... Ts>
void pprint(const Ts&... items)
{
    (std::print("{} ", items), ...);
    std::println();
}

constexpr auto settings = reflexx::serializer_settings::Strict();

// Runtime only
using s = reflexx::serializer<settings, reflexx::backends::YyjsonBackend>;

enum class Role { Admin, Guest, User };

struct Stats {
    int strength = 0;
    int agility = 0;
    float luck = 0.f;
};

struct Inventory {
    std::vector<std::string> items;
    std::vector<int> quantities;

    bool operator==(const Inventory& other) const {
        return items == other.items && quantities == other.quantities;
    }
};

struct Entity {
    std::string name;
    int level = 0;

    bool operator==(const Entity& other) const {
        return name == other.name && level == other.level;
    }
};

struct Player : Entity {
    Role role = Role::User;
    Stats stats;
    Inventory inventory;
    std::vector<std::string> titles;

    bool operator==(const Player& other) const {
        return static_cast<const Entity&>(*this) == static_cast<const Entity&>(other) &&
               role == other.role &&
               stats.strength == other.stats.strength &&
               stats.agility == other.stats.agility &&
               stats.luck == other.stats.luck &&
               inventory == other.inventory &&
               titles == other.titles;
    }
};

TEST_CASE( "Complex", "[MISC]" )
{
    Player p1{
        .role = Role::Admin,
        .stats = Stats{10, 12, 3.14f},
        .inventory = Inventory{ {"sword", "shield"}, {1, 1} },
        .titles = { "Champion", "Legend" }
    };
    p1.name = "Brunko";
    p1.level = 69;

    Player p2;

    auto data = s::serialize(p1);
    s::deserialize(p2, data.get());

    REQUIRE( p1 == p2 );
}


