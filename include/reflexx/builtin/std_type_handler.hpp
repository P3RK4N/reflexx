#ifndef REFLEXX_STD_TYPE_HANDLER_HPP
#define REFLEXX_STD_TYPE_HANDLER_HPP

#include <string>

#include "reflexx/custom_type_handler.hpp"

namespace reflexx {

template <typename TSerializer, bool IsReading>
struct std_type_handler : custom_type_handler<TSerializer, IsReading>
{    
    void serialize(std::string& v)
    {
        this->serialize_string(v);        
    }

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
};

} // reflexx

#endif