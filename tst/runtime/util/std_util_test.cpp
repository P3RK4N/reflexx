#include "test_utils.hpp"

TEST_CASE("reflexx::util::is_std_map and is_std_set traits")
{
    // Maps
    STATIC_REQUIRE(reflexx::util::is_std_map_v<std::map<int,int>>);
    STATIC_REQUIRE(reflexx::util::is_std_map_v<std::multimap<int,int>>);
    STATIC_REQUIRE(reflexx::util::is_std_map_v<std::unordered_map<int,int>>);
    STATIC_REQUIRE(reflexx::util::is_std_map_v<std::unordered_multimap<int,int>>);

    // Not maps
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_map_v<int>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_map_v<std::set<int>>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_map_v<std::vector<int>>);

    // Sets
    STATIC_REQUIRE(reflexx::util::is_std_set_v<std::set<int>>);
    STATIC_REQUIRE(reflexx::util::is_std_set_v<std::multiset<int>>);
    STATIC_REQUIRE(reflexx::util::is_std_set_v<std::unordered_set<int>>);
    STATIC_REQUIRE(reflexx::util::is_std_set_v<std::unordered_multiset<int>>);

    // Not sets
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_set_v<int>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_set_v<std::map<int,int>>);
    STATIC_REQUIRE_FALSE(reflexx::util::is_std_set_v<std::vector<int>>);
}