#ifndef REFLEXX_STD_TYPE_HANDLER_HPP
#define REFLEXX_STD_TYPE_HANDLER_HPP

#include <ctime>
#include <experimental/meta>
#include <string>
#include <type_traits>
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

// TODO: shared_ptr, unique_ptr

namespace reflexx {

template <typename TSerializer, bool IsReading>
struct std_handler : type_handler<TSerializer, IsReading>
{    
    void serialize(std::string& v)
    {
        this->serialize_string(v);        
    }

    template <typename T1, typename T2>
    void serialize(std::pair<T1, T2>& pair)
    {
        this->begin_array();

            this->serialize_object(pair.first);
            this->serialize_object(pair.second);

        this->end_array();
    }

    template <typename T>
    void serialize(std::optional<T>& opt)
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
                    opt.emplace();
                }
                this->serialize_object(*opt);
            }
        }
        else
        {
            if (opt.has_value())
            {
                this->serialize_object(*opt);   
            }
            else
            {
                this->serialize_null();
            }
        }
    }

    template <std::size_t N>
    consteval static std::span<const std::size_t> enumerate()
    {
        std::array<std::size_t, N> arr;

        for (std::size_t i = 0; i < N; ++i)
        {
            arr[i] = i;
        }

        return std::define_static_array(arr);
    }

    template <typename... Ts>
    void serialize(std::tuple<Ts...>& tuple)
    {
        this->begin_array();

        template for (constexpr auto i : enumerate<sizeof...(Ts)>())
        {
            this->serialize_object(std::get<i>(tuple));
        }

        this->end_array();
    }

    template <typename... Types>
    void serialize(std::variant<Types...>& var)
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            std::size_t index{};
            this->serialize_number(index);

            template for (constexpr auto i : enumerate<sizeof...(Types)>())
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
            std::visit([&](auto& value) { this->serialize_object(value); }, var);
        }

        this->end_array();
    }

    /**
     *  ##################################################
     *  ############# SEQUENTIAL CONTAINERS ##############
     *  ##################################################
     */

    template <typename T, typename Alloc>
    void serialize(std::vector<T, Alloc>& vec)
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            vec.clear();
            while (this->has_next())
            {
                vec.emplace_back();
                this->serialize_object(vec.back());
            }
        }
        else
        {
            for (const auto& elem : vec)
            {
                this->serialize_object(elem);
            }
        }

        this->end_array();
    }

    template <typename T, typename Alloc>
    void serialize(std::deque<T, Alloc>& deq)
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            deq.clear();
            while (this->has_next())
            {
                deq.emplace_back();
                this->serialize_object(deq.back());
            }
        }
        else
        {
            for (const auto& elem : deq)
            {
                this->serialize_object(elem);
            }
        }

        this->end_array();
    }

    template <typename T, typename Alloc>
    void serialize(std::list<T, Alloc>& lst)
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            lst.clear();
            while (this->has_next())
            {
                lst.emplace_back();
                this->serialize_object(lst.back());
            }
        }
        else
        {
            for (const auto& elem : lst)
            {
                this->serialize_object(elem);
            }
        }

        this->end_array();
    }

    template <typename T, typename Alloc>
    void serialize(std::forward_list<T, Alloc>& flst)
    {
        this->begin_array();

        if constexpr (IsReading)
        {
            flst.clear();
            auto insert_pos = flst.before_begin();
            while (this->has_next())
            {
                insert_pos = flst.emplace_after(insert_pos);
                this->serialize_object(*insert_pos);
            }
        }
        else
        {
            for (const auto& elem : flst)
            {
                this->serialize_object(elem);
            }
        }

        this->end_array();
    }

    template <typename T, std::size_t N>
    void serialize(std::array<T, N>& arr)
    {
        this->begin_array();

        // MISC: "template for (auto& elem : arr)" not working?
        template for (constexpr auto I : enumerate<N>())
        {
            this->serialize_object(arr[I]);
        }

        this->end_array();
    }

    template <typename T>
    void serialize(std::valarray<T>& arr)
    {
        this->begin_object();
        
            this->key("size");
            std::size_t size = arr.size();
            this->serialize_number(size);

            if constexpr (IsReading)
            {
                arr.resize(size);
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
    void serialize(T& map)
    {
        this->serialize_map(map);
    }

    template <typename T>
    requires util::is_std_set_v<T>
    void serialize(T& set)
    {
        this->serialize_set(set);
    }

    template <typename TMap>
    requires (!std::is_same_v<typename TMap::key_type, std::string>)
    void serialize_map(TMap& map)
    {
        using K = typename TMap::key_type;
        using V = typename TMap::mapped_type;

        this->begin_array();

        if constexpr (IsReading)
        {
            map.clear();
            while (this->has_next())
            {
                std::pair<K, V> entry {};

                this->serialize_object(entry);

                map.insert(std::move(entry));
            }
        }
        else
        {
            for (const auto& entry : map)
            {
                this->serialize_object(entry);
            }
        }

        this->end_array();
    }

    template <typename TMap>
    requires std::is_same_v<typename TMap::key_type, std::string>
    void serialize_map(TMap& map)
    {
        using K = typename TMap::key_type;
        using V = typename TMap::mapped_type;

        this->begin_object();

        if constexpr (IsReading)
        {
            map.clear();

            while (this->has_next())
            {
                std::pair<std::string, V> entry { this->key(), V{} };
                this->serialize_object(entry.second);
                map.emplace(std::move(entry));
            }
        }
        else
        {
            for (const auto& [key, value] : map)
            {
                this->key(key);
                this->serialize_object(value);
            }
        }

        this->end_object();
    }    

    template <typename TSet>
    void serialize_set(TSet& set)
    {
        using T = typename TSet::value_type;

        this->begin_array();

        if constexpr (IsReading)
        {
            set.clear();
            while (this->has_next())
            {
                T value {};
                this->serialize_object(value);
                set.insert(std::move(value));
            }
        }
        else
        {
            for (const auto& value : set)
            {
                this->serialize_object(value);
            }
        }

        this->end_array();
    }
};

} // reflexx

#endif