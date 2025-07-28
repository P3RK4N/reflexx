#ifndef REFLEXX_GRAPH_PTR_HANDLER_HPP
#define REFLEXX_GRAPH_PTR_HANDLER_HPP

#include <memory>
#include <map>

#include "reflexx/provider.hpp"
#include "reflexx/type_handler.hpp"

namespace reflexx {

// Greedy pointer usage
// Assumes exclusivity between unique and shared pointer
// Assumes unique pointers wont end up in cycle

template <typename TSerializer, bool IsReading>
struct ptr_graph_handler : type_handler<TSerializer, IsReading>
{
    std::map<std::uint64_t, std::shared_ptr<void>> handled_objs {};

    template <typename T>
    inline constexpr void serialize(std::shared_ptr<T>& ptr)
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
                serialize_ptr_graph(ptr);
            }
        }
        else
        {
            if (ptr == nullptr)
            {
                this->serialize_null();
            }
            else
            {
                serialize_ptr_graph(ptr);
            }
        }
    }

    template <typename T>
    inline constexpr void serialize_ptr_graph(std::shared_ptr<T>& ptr)
    {
        std::uint64_t ptr_val = reinterpret_cast<std::uint64_t>(ptr.get());

        this->begin_object();

        this->key("__id__");
        this->serialize_number(ptr_val);

        this->key("__val__");
        if constexpr (IsReading)
        {
            if (handled_objs.contains(ptr_val))
            {
                ptr = std::static_pointer_cast<T>(handled_objs[ptr_val]);
                this->skip();
            }
            else
            {
                if (ptr == nullptr)
                {
                    ptr = std::make_shared<T>(provider<T>{}());
                }
                handled_objs.emplace(ptr_val, ptr);
                this->serialize_object(*ptr);
            }
        }
        else
        {
            if (handled_objs.contains(ptr_val))
            {
                this->serialize_null();
            }
            else
            {
                handled_objs.emplace(ptr_val, nullptr);
                this->serialize_object(*ptr);
            }
        }

        this->end_object();
    }

};

} // reflexx

#endif