#include <print>
#include <experimental/meta>
#include <map>
#include <type_traits>

struct mom
{
    int a;
    std::vector<int> b;
};

struct dad
{
    int a;
    std::map<int, int> c;
};

struct child : public mom, public dad
{
    char d;
};

// consteval std::string vectorToString(const std::meta::info info)
// {
//     std::string result = "{ ";
    
//     template for (constexpr auto elem : std::meta::enumerators_of(info))
//     {
//         // if (std::meta::is_class_member(elem))
//         // {
//         //     result += std::meta::identifier_of(elem);
//         //     result += ", ";
//         // }
//     }
//     result += "}";
    
//     return result;
// }

template <typename E>
requires std::is_class_v<E>
consteval std::string func()
{
    auto members = std::meta::members_of(^^E);
    std::string result = "{ ";
    for (const auto& member : members)
    {
        if (std::meta::is_nonstatic_data_member(member))
        {
            result += std::meta::identifier_of(member);
            result += ", ";
        }
    }
    result += "}";
    return result;
}

int main()
{
    std::println("Hello, World!");
    constexpr std::meta::info a = ^^child;

    std::println("members_of: {}", func<child>());

    // consteval auto b = vectorToString(std::meta::members_of(a));
    // std::println("identifier_of: {}", std::meta::identifier_of(a)); 
    // std::println("is_class_member: {}", (std::meta::is_class_member(a)));
    // std::println("is_base: {}", (std::meta::is_base(a)));
    // std::println("is_object: {}", (std::meta::is_object(a)));
    // std::println("parameters_of: {}", );
    // std::println("parent_of: {}", (std::meta::parent_of(a)));
    // std::println("bases_of: {}", (std::meta::bases_of(a)));
    // std::println("nonstatic_data_members_of: {}", (std::meta::nonstatic_data_members_of(a)));

    return 0;
}