#include "test_utils.hpp"

template <typename TSerializer, typename T>
void roundtrip_check_all(const T& value)
{
    auto type = std::meta::display_string_of(std::meta::dealias(^^T));

    // As root values
    SECTION(std::format("{}", type))
    {
        roundtrip_check<TSerializer, T>(value);
    }

    SECTION(std::format("optional<{}> - defined", type))
    {
        roundtrip_check<TSerializer, std::optional<T>>(value);
    }

    SECTION(std::format("optional<{}> - empty", type))
    {
        roundtrip_check<TSerializer, std::optional<T>>(std::nullopt);
    }

    // As values inside sequence
    SECTION(std::format("std::vector<{}> - size 0", type))
    {
        roundtrip_check<TSerializer, std::vector<T>>({});
    }

    SECTION(std::format("std::vector<{}> - size 1", type))
    {
        roundtrip_check<TSerializer, std::vector<T>>({ value });
    }

    SECTION(std::format("std::vector<{}> - size 3", type))
    {
        roundtrip_check<TSerializer, std::vector<T>>({ value, value, value });
    }

    // As value inside sequence of sequences
    SECTION(std::format("std::array<std::vector<{}>, 3>", type))
    {
        std::array<std::vector<T>, 3> arr;
        arr[0] = std::vector<T>{};
        arr[1] = std::vector<T>{ value };
        arr[2] = std::vector<T>{ value, value };
        roundtrip_check<TSerializer, decltype(arr)>(arr);
    }

    // As values inside map
    SECTION(std::format("std::map<std::string, {}>", type))
    {
        std::map<std::string, T> m;
        m.emplace("zero", value);
        m.emplace("one",  value);
        m.emplace("two",  value);
        roundtrip_check<TSerializer, decltype(m)>(m);
    }

    // As values inside sequence of maps
    SECTION(std::format("std::array<std::map<std::string, {}>, 3>", type))
    {
        std::array<std::map<std::string, T>, 3> arr;
        for (std::size_t ai = 0; ai < arr.size(); ++ai) {
            auto& m = arr[ai];
            m.emplace("k0", value);
            m.emplace("k1", value);
        }
        roundtrip_check<TSerializer, decltype(arr)>(arr);
    }

    SECTION(std::format("std::array<TestTemplatedStruct<{}>, 3>", type))
    {
        std::array<TestTemplatedStruct<T>, 3> arr;
        arr[0] = TestTemplatedStruct<T>{ value };
        arr[1] = TestTemplatedStruct<T>{ value };
        arr[2] = TestTemplatedStruct<T>{ value };
        roundtrip_check<TSerializer, decltype(arr)>(arr);
    }

    // As values inside map of sequences + extra scenarios
    SECTION(std::format("complex_struct<{}>", type))
    {
        struct complex_struct_t
        {
            std::map<std::string, T> m;
            TestTemplatedStruct<T> templ;
            T value;
            std::optional<T> opt;
            std::vector<T> vec;

            bool operator==(const complex_struct_t& o) const = default;
        };

        complex_struct_t cs;
        cs.m.emplace("a", value);
        cs.m.emplace("b", value);
        cs.templ = TestTemplatedStruct<T>{ value };
        cs.value = value;
        cs.opt   = value;
        cs.vec   = { value, value, value };

        roundtrip_check<TSerializer, complex_struct_t>(cs);
    }
}

struct copyable_unique_ptr
{
    std::unique_ptr<int> ptr;

    copyable_unique_ptr() = default;
    copyable_unique_ptr(std::unique_ptr<int>&& ptr) : ptr(std::move(ptr)) {}
    copyable_unique_ptr(const copyable_unique_ptr& other)
        : ptr(other.ptr ? std::make_unique<int>(*other.ptr) : nullptr) {}

    copyable_unique_ptr& operator=(const copyable_unique_ptr& other)
    {
        if (this != &other)
        {
            ptr = other.ptr ? std::make_unique<int>(*other.ptr) : nullptr;
        }
        return *this;
    }

    bool operator==(const copyable_unique_ptr& other) const
    {
        if (!ptr && !other.ptr) return true;
        if (!ptr || !other.ptr) return false;
        return *ptr == *other.ptr;
    }
};

template <typename T>
struct comparable_shared_ptr
{
    std::shared_ptr<T> ptr;

    comparable_shared_ptr() = default;
    explicit comparable_shared_ptr(T* raw) : ptr(raw) {}
    explicit comparable_shared_ptr(std::shared_ptr<T> p) : ptr(std::move(p)) {}
    comparable_shared_ptr(const comparable_shared_ptr& other) = default;
    comparable_shared_ptr(comparable_shared_ptr&& other) noexcept = default;

    comparable_shared_ptr& operator=(const comparable_shared_ptr& other) = default;
    comparable_shared_ptr& operator=(comparable_shared_ptr&& other) noexcept = default;

    bool operator==(const comparable_shared_ptr& other) const
    {
        if (!ptr && !other.ptr) return true;
        if (!ptr || !other.ptr) return false;
        return *ptr == *other.ptr;
    }
};

struct comparable_val_array
{
    std::valarray<int> v;

    bool operator==(const comparable_val_array& other) const
    {
        return v.size() == other.v.size() &&
            std::equal(std::begin(v), std::end(v), std::begin(other.v));
    }
};

TEMPLATE_LIST_TEST_CASE("std::string - non empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::string>("Ante");
}

TEMPLATE_LIST_TEST_CASE("std::string - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::string>("");
}

TEMPLATE_LIST_TEST_CASE("std::string_view - non empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::string_view>("Hello");
}

TEMPLATE_LIST_TEST_CASE("std::string_view - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::string_view>("");
}

TEMPLATE_LIST_TEST_CASE("std::optional<int> - engaged", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::optional<int>>(42);
}

// NOTE: optional<optional<T>> is UB (look up std_handler for details)
// TODO: Static assert in std_handler to prevent this
// TEMPLATE_LIST_TEST_CASE("std::optional<int> - empty", "", serializers_list)
// {
//     using T = TestType;
//     roundtrip_check_all<T, std::optional<int>>(std::nullopt);
// }

TEMPLATE_LIST_TEST_CASE("std::shared_ptr<int> - non null", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, comparable_shared_ptr<int>>(comparable_shared_ptr{ std::make_shared<int>(1337) });
}

TEMPLATE_LIST_TEST_CASE("std::shared_ptr<int> - null", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, comparable_shared_ptr<int>>(comparable_shared_ptr{ std::shared_ptr<int>{} });
}

TEMPLATE_LIST_TEST_CASE("std::unique_ptr<int> - non null", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, copyable_unique_ptr>(copyable_unique_ptr{ std::make_unique<int>(1337) });
}

TEMPLATE_LIST_TEST_CASE("std::unique_ptr<int> - null", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, copyable_unique_ptr>(copyable_unique_ptr{});
}

TEMPLATE_LIST_TEST_CASE("std::vector<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::vector<int>>(std::vector<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::vector<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::vector<int>>(std::vector<int>{1});
}

TEMPLATE_LIST_TEST_CASE("std::vector<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::vector<int>>(std::vector<int>{1,2,3});
}

TEMPLATE_LIST_TEST_CASE("std::deque<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::deque<int>>(std::deque<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::deque<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::deque<int>>(std::deque<int>{1});
}

TEMPLATE_LIST_TEST_CASE("std::deque<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::deque<int>>(std::deque<int>{1,2,3});
}

TEMPLATE_LIST_TEST_CASE("std::list<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::list<int>>(std::list<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::list<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::list<int>>(std::list<int>{1});
}

TEMPLATE_LIST_TEST_CASE("std::list<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::list<int>>(std::list<int>{1,2,3});
}

TEMPLATE_LIST_TEST_CASE("std::forward_list<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::forward_list<int>>(std::forward_list<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::forward_list<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::forward_list<int>>(std::forward_list<int>{1});
}

TEMPLATE_LIST_TEST_CASE("std::forward_list<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::forward_list<int>>(std::forward_list<int>{1,2,3});
}

TEMPLATE_LIST_TEST_CASE("std::array<int,3>", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::array<int,3>>(std::array<int,3>{10,20,30});
}

TEMPLATE_LIST_TEST_CASE("std::valarray<int>", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, comparable_val_array>(comparable_val_array{ std::valarray<int>{10,20,30} });
}

TEMPLATE_LIST_TEST_CASE("std::pair<int,std::string>", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::pair<int,std::string>>(std::pair<int,std::string>{42,"world"});
}

TEMPLATE_LIST_TEST_CASE("std::tuple<int,std::string,bool>", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::tuple<int,std::string,bool>>(std::tuple<int,std::string,bool>{1,"test",true});
}

TEMPLATE_LIST_TEST_CASE("std::variant<int,std::string> - int", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::variant<int,std::string>>(42);
}

TEMPLATE_LIST_TEST_CASE("std::variant<int,std::string> - string", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::variant<int,std::string>>("variant");
}

// std::map<std::string,int>

TEMPLATE_LIST_TEST_CASE("std::map<std::string,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<std::string,int>>(std::map<std::string,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::map<std::string,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<std::string,int>>(std::map<std::string,int>{{"a",1}});
}

TEMPLATE_LIST_TEST_CASE("std::map<std::string,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<std::string,int>>(std::map<std::string,int>{{"a",1},{"b",2}});
}

// unordered_map
TEMPLATE_LIST_TEST_CASE("std::unordered_map<std::string,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<std::string,int>>(std::unordered_map<std::string,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_map<std::string,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<std::string,int>>(std::unordered_map<std::string,int>{{"a",1}});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_map<std::string,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<std::string,int>>(std::unordered_map<std::string,int>{{"a",1},{"b",2}});
}

// map<int,int>
TEMPLATE_LIST_TEST_CASE("std::map<int,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<int,int>>(std::map<int,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::map<int,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<int,int>>(std::map<int,int>{{1,1}});
}

TEMPLATE_LIST_TEST_CASE("std::map<int,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::map<int,int>>(std::map<int,int>{{1,1},{2,2}});
}

// unordered_map<int,int>
TEMPLATE_LIST_TEST_CASE("std::unordered_map<int,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<int,int>>(std::unordered_map<int,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_map<int,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<int,int>>(std::unordered_map<int,int>{{1,1}});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_map<int,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_map<int,int>>(std::unordered_map<int,int>{{1,1},{2,2}});
}

// multimap<std::string,int>
TEMPLATE_LIST_TEST_CASE("std::multimap<std::string,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<std::string,int>>(std::multimap<std::string,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::multimap<std::string,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<std::string,int>>(std::multimap<std::string,int>{{"a",1}});
}

TEMPLATE_LIST_TEST_CASE("std::multimap<std::string,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<std::string,int>>(std::multimap<std::string,int>{{"a",1},{"b",2},{"a",3}});
}

// multimap<int,int>
TEMPLATE_LIST_TEST_CASE("std::multimap<int,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<int,int>>(std::multimap<int,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::multimap<int,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<int,int>>(std::multimap<int,int>{{1,1}});
}

TEMPLATE_LIST_TEST_CASE("std::multimap<int,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multimap<int,int>>(std::multimap<int,int>{{1,1},{2,2},{1,3}});
}

// unordered_multimap<std::string,int>
TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<std::string,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<std::string,int>>(std::unordered_multimap<std::string,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<std::string,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<std::string,int>>(std::unordered_multimap<std::string,int>{{"a",1}});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<std::string,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<std::string,int>>(std::unordered_multimap<std::string,int>{{"a",1},{"b",2},{"a",3}});
}

// unordered_multimap<int,int>
TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<int,int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<int,int>>(std::unordered_multimap<int,int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<int,int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<int,int>>(std::unordered_multimap<int,int>{{1,1}});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multimap<int,int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multimap<int,int>>(std::unordered_multimap<int,int>{{1,1},{2,2},{1,3}});
}

// set<int>
TEMPLATE_LIST_TEST_CASE("std::set<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::set<int>>(std::set<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::set<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::set<int>>(std::set<int>{42});
}

TEMPLATE_LIST_TEST_CASE("std::set<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::set<int>>(std::set<int>{10,20,30});
}

// unordered_set<int>
TEMPLATE_LIST_TEST_CASE("std::unordered_set<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_set<int>>(std::unordered_set<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_set<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_set<int>>(std::unordered_set<int>{42});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_set<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_set<int>>(std::unordered_set<int>{10,20,30});
}

// multiset<int>
TEMPLATE_LIST_TEST_CASE("std::multiset<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multiset<int>>(std::multiset<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::multiset<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multiset<int>>(std::multiset<int>{42});
}

TEMPLATE_LIST_TEST_CASE("std::multiset<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::multiset<int>>(std::multiset<int>{10,20,20,30});
}

// unordered_multiset<int>
TEMPLATE_LIST_TEST_CASE("std::unordered_multiset<int> - empty", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multiset<int>>(std::unordered_multiset<int>{});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multiset<int> - single", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multiset<int>>(std::unordered_multiset<int>{42});
}

TEMPLATE_LIST_TEST_CASE("std::unordered_multiset<int> - multi", "", serializers_list)
{
    using T = TestType;
    roundtrip_check_all<T, std::unordered_multiset<int>>(std::unordered_multiset<int>{10,20,20,30});
}