#ifndef REFLEXX_STD_TYPE_HANDLER_HPP
#define REFLEXX_STD_TYPE_HANDLER_HPP

#include <ctime>
#include <experimental/meta>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <array>
#include <optional>
#include <tuple>
#include <variant>
#include <valarray>

#include "reflexx/type_handler.hpp"
#include "reflexx/util/std_util.hpp"
#include "reflexx/provider.hpp"
#include "reflexx/util/enumerate.hpp"

// TODO: Handle noexcept?
// TODO: Keep everything constexpr?

namespace reflexx {

template <typename TSerializer, bool IsReading>
struct std_handler : type_handler<TSerializer, IsReading>
{    
    template <typename T1, typename T2>
    inline constexpr void serialize(std::pair<T1, T2>& pair) const
    noexcept(
        noexcept(this->begin_array()) && 
        noexcept(this->end_array()) &&
        noexcept(this->serialize_object(pair.first)) &&
        noexcept(this->serialize_object(pair.second))
    )
    {
        this->begin_array();

            this->serialize_object(pair.first);
            this->serialize_object(pair.second);

        this->end_array();
    }

    // Greedy optional usage
    // NOTE: Having nested optionals (e.g. optional<optional<T>>) is UB
    //       Because there is no way to tell if inner or outer optional is empty.
    //       If you want to support that, write your own handler which stores 'empty' flag too.
    template <typename T>
    inline constexpr void serialize(std::optional<T>& opt) const
    {
        if constexpr (IsReading)
        {
            if (this->is_null())
            {
                opt.reset();
                this->skip();
            }
            else
            {
                if (!opt.has_value())
                {
                    opt.emplace(provider<T>{}());
                }
                this->serialize_object(*opt);
            }
        }
        else
        {
            opt.has_value() ? this->serialize_object(*opt) : this->serialize_null();
        }
    }

    // Greedy pointer usage
    // NOTE: Look at std::optional handler. Same UB applies here.
    template <typename T>
    inline constexpr void serialize(std::shared_ptr<T>& ptr) const
    {
        if constexpr (IsReading)
        {
            if (this->is_null())
            {
                ptr.reset();
                this->skip();
            }
            else
            {
                if (ptr == nullptr)
                {
                    ptr = std::make_shared<T>(provider<T>{}());
                }
                this->serialize_object(*ptr);
            }
        }
        else
        {
            ptr == nullptr ? this->serialize_null() : this->serialize_object(*ptr);
        }
    }

    // Greedy pointer usage
    // NOTE: Look at std::optional handler. Same UB applies here.
    template <typename T>
    inline constexpr void serialize(std::unique_ptr<T>& ptr) const
    {
        if constexpr (IsReading)
        {
            if (this->is_null())
            {
                ptr.reset();
                this->skip();
            }
            else
            {
                if (ptr == nullptr)
                {
                    ptr = std::make_unique<T>(provider<T>{}());
                }
                this->serialize_object(*ptr);
            }
        }
        else
        {
            ptr == nullptr ? this->serialize_null() : this->serialize_object(*ptr);
        }
    }

    template <typename... Ts>
    inline constexpr void serialize(std::tuple<Ts...>& tuple) const
    {
        this->begin_array();

        template for (constexpr auto i : util::enumerate<sizeof...(Ts)>)
        {
            this->serialize_object(std::get<i>(tuple));
        }

        this->end_array();
    }

    template <typename... Types>
    inline constexpr void serialize(std::variant<Types...>& var) const
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            std::size_t index{};
            this->serialize_number(index);

            template for (constexpr auto i : util::enumerate<sizeof...(Types)>)
            {
                if (i == index)
                {
                    var.template emplace<i>();
                    this->serialize_object(std::get<i>(var));
                    break;
                }
            }
        }
        else
        {
            this->serialize_number(var.index());
            std::visit([this](auto& value) { this->serialize_object(value); }, var);
        }

        this->end_array();
    }

    /**
     *  ##################################################
     *  ############# SEQUENTIAL CONTAINERS ##############
     *  ##################################################
     */

    template <typename T, typename Alloc>
    inline constexpr void serialize(std::vector<T, Alloc>& vec) const
    {
        
        if constexpr (IsReading)
        {
            vec.clear();            
            this->arr_foreach([&]()
            {
                vec.emplace_back(provider<T>{}());
                this->serialize_object(vec.back());    
            });
        }
        else
        {
            this->begin_array();
            for (const auto& elem : vec)
            {
                this->serialize_object(elem);
            }
            this->end_array();
        }
    }

    template <typename T, typename Alloc>
    inline constexpr void serialize(std::deque<T, Alloc>& deq) const
    {
        if constexpr (IsReading)
        {
            deq.clear();
            this->arr_foreach([&]()
            {
                deq.emplace_back(provider<T>{}());
                this->serialize_object(deq.back());    
            });
        }
        else
        {
            this->begin_array();
            for (const auto& elem : deq)
            {
                this->serialize_object(elem);
            }
            this->end_array();
        }
    }

    template <typename T, typename Alloc>
    inline constexpr void serialize(std::list<T, Alloc>& lst) const
    {
        if constexpr (IsReading)
        {
            lst.clear();
            this->arr_foreach([&]()
            {
                lst.emplace_back(provider<T>{}());
                this->serialize_object(lst.back());                
            });
        }
        else
        {
            this->begin_array();
            for (const auto& elem : lst)
            {
                this->serialize_object(elem);
            }
            this->end_array();
        }

    }

    template <typename T, typename Alloc>
    inline constexpr void serialize(std::forward_list<T, Alloc>& flst) const
    {
        if constexpr (IsReading)
        {
            flst.clear();
            auto insert_pos = flst.before_begin();
            this->arr_foreach([&]()
            {
                insert_pos = flst.emplace_after(insert_pos, provider<T>{}());
                this->serialize_object(*insert_pos);                
            });
        }
        else
        {
            this->begin_array();
            for (const auto& elem : flst)
            {
                this->serialize_object(elem);
            }
            this->end_array();
        }
    }

    template <typename T, std::size_t N>
    inline constexpr void serialize(std::array<T, N>& arr) const
    {
        this->begin_array();

        // MISC: "template for (auto& elem : arr)" not working?
        template for (constexpr auto I : util::enumerate<N>)
        {
            this->serialize_object(arr[I]);
        }

        this->end_array();
    }

    template <typename T>
    inline constexpr void serialize(std::valarray<T>& arr) const
    {
        this->begin_object();
        
            this->key("size");
            std::size_t size = arr.size();
            this->serialize_number(size);

            if constexpr (IsReading)
            {
                arr.resize(size, provider<T>{}());
            }

            this->key("data");
            this->begin_array();
                for (auto& elem : arr)
                {
                    this->serialize_object(elem);
                }
            this->end_array();

        this->end_object();
    }

    /**
     *  ##################################################
     *  ############# ASSOCIATIVE CONTAINERS #############
     *  ##################################################
     */

    template <typename T>
    requires util::is_std_map_v<T>
    inline constexpr void serialize(T& map) const
    {
        this->serialize_map(map);
    }

    template <typename T>
    requires util::is_std_set_v<T>
    inline constexpr void serialize(T& set) const
    {
        this->serialize_set(set);
    }

    template <typename TMap>
    requires (!std::is_same_v<typename TMap::key_type, std::string> && !std::is_same_v<typename TMap::key_type, std::string_view>)
    inline constexpr void serialize_map(TMap& map) const
    {
        using K = typename TMap::key_type;
        using V = typename TMap::mapped_type;

        if constexpr (IsReading)
        {
            map.clear();
            this->arr_foreach([&]()
            {
                K k = provider<K>{}();
                V v = provider<V>{}();
                
                this->begin_array();
                this->serialize_object(k);
                this->serialize_object(v);
                this->end_array();
                
                map.emplace(std::move(k), std::move(v));
                
            });
        }
        else
        {
            this->begin_array();
            for (const auto& entry : map)
            {
                this->serialize_object(entry);
            }
            this->end_array();
        }
    }

    template <typename TMap>
    requires std::is_same_v<typename TMap::key_type, std::string> || std::is_same_v<typename TMap::key_type, std::string_view>
    inline constexpr void serialize_map(TMap& map) const
    {
        using K = typename TMap::key_type;
        using V = typename TMap::mapped_type;

        if constexpr (IsReading)
        {
            map.clear();
            
            this->obj_foreach([&](auto&& key)
            {
                V v = provider<V>{}();
                this->serialize_object(v);
                map.emplace(std::move(key), std::move(v));
            });
        }
        else
        {
            this->begin_object();
            for (const auto& [key, value] : map)
            {
                this->key(key);
                this->serialize_object(value);
            }
            this->end_object();
        }
    }    

    template <typename TSet>
    inline constexpr void serialize_set(TSet& set) const
    {
        using T = typename TSet::value_type;

        if constexpr (IsReading)
        {
            set.clear();
            this->arr_foreach([&]()
            {
                T value = provider<T>{}();
                this->serialize_object(value);
                set.emplace(std::move(value));                
            });
        }
        else
        {
            this->begin_array();
            for (const auto& value : set)
            {
                this->serialize_object(value);
            }
            this->end_array();
        }
    }
};

} // reflexx

#endif