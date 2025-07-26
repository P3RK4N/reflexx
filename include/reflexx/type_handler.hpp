#ifndef REFLEXX_TYPE_HANDLER_HPP
#define REFLEXX_TYPE_HANDLER_HPP

#include <string_view>

#include "reflexx/declare.hpp"

// TODO: Update with <stdfloat>

namespace reflexx
{

template <IsSerializer TSerializer, bool IsReading>
class type_handler
{
private:
    friend TSerializer;
    static constexpr bool IsWriting = !IsReading;

    typename TSerializer::serializer_context* __ctx__;
    
protected:
    template <typename T>
    constexpr inline void serialize_object(T& obj) const requires IsReading 
    { 
        TSerializer::deserialize(obj, *__ctx__);
    }

    template <typename T>
    constexpr inline void serialize_object(const T& obj) const requires IsWriting
    { 
        TSerializer::serialize(obj, *__ctx__);
    }

    constexpr inline void key(std::string_view key) const 
    {
        if constexpr (IsReading)
        {
            __ctx__->backend_->read_key(key);
        }
        else
        {
            __ctx__->backend_->write_key(key);
        }
    }

    constexpr inline void begin_array() const 
    {
        if constexpr (IsReading)
        {
            __ctx__->backend_->read_begin_array();
        }
        else
        {
            __ctx__->backend_->write_begin_array();
        }
    }

    constexpr inline void end_array() const 
    { 
        if constexpr (IsReading)
        {
            __ctx__->backend_->read_end_array();
        }
        else
        {
            __ctx__->backend_->write_end_array();
        }
    }

    constexpr inline void begin_object() const 
    { 
        if constexpr (IsReading)
        {
            __ctx__->backend_->read_begin_object();
        }
        else
        {
            __ctx__->backend_->write_begin_object();
        }
    }

    constexpr inline void end_object() const 
    { 
        if constexpr (IsReading)
        {
            __ctx__->backend_->read_end_object();
        }
        else
        {
            __ctx__->backend_->write_end_object();
        }
    }

    constexpr inline void serialize_bool(bool& v) const requires IsReading 
    { 
        __ctx__->backend_->read_bool(v);
    }

    constexpr inline void serialize_bool(bool v) const requires IsWriting
    { 
        __ctx__->backend_->write_bool(v);
    }

    constexpr inline void serialize_string(std::string& v) const requires IsReading
    { 
        v = __ctx__->backend_->read_string();
    }

    constexpr inline void serialize_string(std::string_view v) const requires IsWriting 
    { 
        __ctx__->backend_->write_string(v);
    }
    
    template <typename T>
    requires std::is_arithmetic_v<T>
    constexpr inline void serialize_number(T value) const requires IsWriting
    {
        if      constexpr (std::is_same_v<T, std::int8_t>)      __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::int16_t>)     __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::int32_t>)     __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::int64_t>)     __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::uint8_t>)     __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::uint16_t>)    __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::uint32_t>)    __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, std::uint64_t>)    __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, float>)            __ctx__->backend_->write_number(value);
        else if constexpr (std::is_same_v<T, double>)           __ctx__->backend_->write_number(value);
        else static_assert(false, "Unsupported number type in write_number");
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    constexpr inline void serialize_number(T& value) const requires IsReading
    {
        if      constexpr (std::is_same_v<T, std::int8_t>)      __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::int16_t>)     __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::int32_t>)     __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::int64_t>)     __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::uint8_t>)     __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::uint16_t>)    __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::uint32_t>)    __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, std::uint64_t>)    __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, float>)            __ctx__->backend_->read_number(value);
        else if constexpr (std::is_same_v<T, double>)           __ctx__->backend_->read_number(value);
        else static_assert(false, "Unsupported number type in read_number");
    }

    /**
     * ###################################################
     * ############## WRITE ONLY METHODS #################
     * ###################################################
     */

    constexpr inline void serialize_null() const 
    { 
        static_assert(IsWriting, "serialize_null is only callable in Write/Serialize mode!");
        __ctx__->backend_->write_null();
    }

    /**
     * ###################################################
     * ############## READ ONLY METHODS ##################
     * ###################################################
     */

    constexpr inline bool is_null() const 
    {
        static_assert(IsReading, "is_null is only callable in Read/Deserialize mode!");
        return __ctx__->backend_->read_is_null();
    }

    constexpr inline void skip() const
    {
        static_assert(IsReading, "skip is only callable in Read/Deserialize mode!");
        return __ctx__->backend_->read_skip(); 
    }

    constexpr inline bool has_next() const
    {
        static_assert(IsReading, "has_next is only callable in Read/Deserialize mode!");
        return __ctx__->backend_->read_has_next();
    }

    constexpr inline std::string_view key() const
    {
        static_assert(IsReading, "key(void) is only callable in Read/Deserialize mode!");
        return __ctx__->backend_->read_key();
    }
};

}

#endif