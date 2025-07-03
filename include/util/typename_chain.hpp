#ifndef TYPENAME_CHAIN_HPP
#define TYPENAME_CHAIN_HPP

#include <cstddef>
#include <experimental/meta>
#include <tuple>
#include <string_view>

namespace reflexx {
namespace util {
namespace detail {

template <std::size_t N, typename... Ts>
struct at
{
    using type = std::tuple_element_t<N, std::tuple<Ts...>>;
};

template <std::size_t N>
struct at<N>
{
    using type = void;
};

} // detail

template <typename... Ts>
struct typename_chain
{    
    static constexpr std::size_t size = sizeof...(Ts);
    using type = typename_chain<Ts...>;

    template <typename... Us>
    using append = typename_chain<Ts..., Us...>;
    
    template <std::size_t N>
    using at = typename detail::at<N, Ts...>::type;

    using front = at<0>;
    using back  = at<size - 1>;
    
    template <std::size_t N>
    using ltrim = decltype([]<std::size_t... Is>(std::index_sequence<Is...>) {
        return typename_chain<at<N + Is>...>{};
    }(std::make_index_sequence<size - N>{}))::type;
    
    template <std::size_t Begin, std::size_t End>
    using slice = decltype([]<std::size_t... Is>(std::index_sequence<Is...>)
    {
        return typename_chain<at<Begin + Is>...>{};
    }(std::make_index_sequence<End - Begin>{}))::type;

    static consteval std::string_view view()
    {
        if constexpr (size == 0)
        {
            return "";
        }

        constexpr std::size_t       view_size       = ( std::meta::display_string_of(^^Ts).size() + ... + 0 ) + size; 
        constexpr std::string_view  identifiers[]   = { std::meta::display_string_of(^^Ts)... };
        char buff[view_size];
        
        std::size_t pos = 0;
        for (std::size_t i = 0; i < size; i++)
        {
            for (auto c : identifiers[i])
            {
                buff[pos++] = c;
            }
            if (i + 1 < size)
            {
                buff[pos++] = '.';
            }
        }
        
        return std::define_static_string({ buff, pos });
    }

    template <std::meta::info MemberInfo>
    requires (std::meta::is_nonstatic_data_member(MemberInfo))
    static consteval std::string_view view_with_member()
    {
        constexpr std::string_view member_view = std::meta::identifier_of(MemberInfo);

        if constexpr (size == 0)
        {
            return member_view;
        }

        char buff[view().size() + 1 + member_view.size()] {};

        std::size_t pos = 0;
        for (char c : view())
        {
            buff[pos++] = c;
        }
        buff[pos++] = '.';
        for (char c : member_view)
        {
            buff[pos++] = c;
        }

        return std::define_static_string({ buff, pos });
    }
    
};

} // util
} // reflexx

#endif