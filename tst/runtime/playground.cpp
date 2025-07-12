#include <cstring>
#include <experimental/meta>
#include <print>

#include <catch2/catch_all.hpp>

#include "reflexx/declare.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/serializer.hpp"
#include "reflexx/backends/placeholder.hpp"
#include "reflexx/backends/yyjson_backend.hpp"
#include "reflexx/type_handler_list.hpp"

template <typename... Ts>
void pyprint(const Ts&... items)
{
    (std::print("{} ", items), ...);
    std::println();
}

constexpr auto settings = reflexx::serializer_settings::Strict();
using s = reflexx::serializer<settings, reflexx::backends::YyjsonBackend>;


TEST_CASE( "Playground", "[MISC]" )
{
    // pyprint(*s::serialize(std::vector<int>{1, 2, 3}));                
    // pyprint(*s::serialize(std::deque<int>{4, 5, 6}));
    // pyprint(*s::serialize(std::list<int>{7, 8, 9}));
    // pyprint(*s::serialize(std::forward_list<int>{10, 11, 12}));
    // pyprint(*s::serialize(std::array<int, 3>{13, 14, 15}));

    // // Define all 8 containers with example data
    // std::map<int, std::string>                 m1  {{1,"one"}, {2,"two"}, {3,"three"}};
    // std::multimap<int, std::string>            mm1 {{1,"one"}, {1,"uno"}, {2,"two"}};
    // std::unordered_map<int, std::string>       um1 {{1,"one"}, {2,"two"}, {3,"three"}};
    // std::unordered_multimap<int, std::string>  umm1{{1,"one"}, {1,"uno"}, {2,"two"}};

    // std::set<int>                              s1  {1, 2, 3};
    // std::multiset<int>                         ms1 {1, 1, 2, 3};
    // std::unordered_set<int>                    us1 {1, 2, 3};
    // std::unordered_multiset<int>               ums1{1, 1, 2, 3};

    // // Print all containers
    // pyprint(*s::serialize(m1));
    // pyprint(*s::serialize(mm1));
    // pyprint(*s::serialize(um1));
    // pyprint(*s::serialize(umm1));

    // pyprint(*s::serialize(s1));
    // pyprint(*s::serialize(ms1));
    // pyprint(*s::serialize(us1));
    // pyprint(*s::serialize(ums1));

    // std::tuple<int, std::string, float> tup{42, "hello", 3.14f};
    // pyprint(*s::serialize(tup)); 

    // std::valarray<int> arr = {1, 2, 3, 4, 5};

    // std::variant<int, std::string, float> v = std::string("banana");
    // std::visit([](const auto& val){ pyprint(val); }, v);
    // pyprint(*s::serialize(v));

    // std::string a = "ivan";

    // pyprint(*s::serialize(2));
}