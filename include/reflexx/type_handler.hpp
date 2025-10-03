#ifndef REFLEXX_TYPE_HANDLER_HPP
#define REFLEXX_TYPE_HANDLER_HPP

#include <string_view>

#include "reflexx/declare.hpp"

namespace reflexx
{

template <IsSerializer TSerializer, bool IsReading>
class type_handler
{
private:
    friend TSerializer;
    using serializer_context_t = typename TSerializer::template serializer_context<IsReading>;
    static constexpr bool IsWriting = !IsReading;

    serializer_context_t* __ctx__;
    
protected:
    template <typename T>
    constexpr inline void serialize_object(T& obj) const 
    noexcept(noexcept(TSerializer::deserialize(obj, *__ctx__))) requires IsReading
    {
        TSerializer::deserialize(obj, *__ctx__);
    }

    template <typename T>
    constexpr inline void serialize_object(const T& obj) const
    noexcept(noexcept(TSerializer::serialize(obj, *__ctx__))) requires IsWriting
    { 
        TSerializer::serialize(obj, *__ctx__);
    }

    constexpr inline void key(std::string_view key) const 
    noexcept(noexcept(__ctx__->backend_.read_key(key))) requires IsReading
    {
        __ctx__->backend_.read_key(key);
    }

    constexpr inline void key(std::string_view key) const
    noexcept(noexcept(__ctx__->backend_.write_key(key))) requires IsWriting
    {
        __ctx__->backend_.write_key(key);
    }

    constexpr inline void begin_array() const
    noexcept(noexcept(__ctx__->backend_.read_begin_array())) requires IsReading
    {
        __ctx__->backend_.read_begin_array();
    }

    constexpr inline void begin_array() const
    noexcept(noexcept(__ctx__->backend_.write_begin_array())) requires IsWriting
    {
        __ctx__->backend_.write_begin_array();
    }

    constexpr inline void end_array() const
    noexcept(noexcept(__ctx__->backend_.read_end_array())) requires IsReading
    {
        __ctx__->backend_.read_end_array();
    }

    constexpr inline void end_array() const
    noexcept(noexcept(__ctx__->backend_.write_end_array())) requires IsWriting
    {
        __ctx__->backend_.write_end_array();
    }

    constexpr inline void begin_object() const
    noexcept(noexcept(__ctx__->backend_.read_begin_object())) requires IsReading
    {
        __ctx__->backend_.read_begin_object();
    }

    constexpr inline void begin_object() const
    noexcept(noexcept(__ctx__->backend_.write_begin_object())) requires IsWriting
    {
        __ctx__->backend_.write_begin_object();
    }

    constexpr inline void end_object() const
    noexcept(noexcept(__ctx__->backend_.read_end_object())) requires IsReading
    {
        __ctx__->backend_.read_end_object();
    }

    constexpr inline void end_object() const
    noexcept(noexcept(__ctx__->backend_.write_end_object())) requires IsWriting
    {
        __ctx__->backend_.write_end_object();
    }

    constexpr inline void serialize_bool(bool& v) const
    noexcept(noexcept(__ctx__->backend_.read_bool(v))) requires IsReading
    {
        __ctx__->backend_.read_bool(v);
    }

    constexpr inline void serialize_bool(const bool v) const
    noexcept(noexcept(__ctx__->backend_.write_bool(v))) requires IsWriting
    {
        __ctx__->backend_.write_bool(v);
    }

    constexpr inline void serialize_string(std::string& v) const
    noexcept(noexcept(v = __ctx__->backend_.read_string())) requires IsReading
    { 
        v = __ctx__->backend_.read_string();
    }

    constexpr inline void serialize_string(const std::string& v) const
    noexcept(noexcept(__ctx__->backend_.write_string(v))) requires IsWriting
    {
        __ctx__->backend_.write_string(v);
    }

    constexpr inline void serialize_string(std::string_view& sv) const
    noexcept(noexcept(sv = __ctx__->backend_.read_string())) requires IsReading
    { 
        sv = __ctx__->backend_.read_string();
    }

    constexpr inline void serialize_string(const std::string_view& sv) const
    noexcept(noexcept(__ctx__->backend_.write_string(sv))) requires IsWriting
    {
        __ctx__->backend_.write_string(sv);
    }

    template <util::is_serializable_number T>
    constexpr inline void serialize_number(T& value) const 
    noexcept(noexcept(__ctx__->backend_.write_number(value))) requires IsReading
    {
        __ctx__->backend_.read_number(value);
    }

    template <util::is_serializable_number T>
    constexpr inline void serialize_number(const T value) const 
    noexcept(noexcept(__ctx__->backend_.write_number(value))) requires IsWriting
    {
        __ctx__->backend_.write_number(value);
    }

    /**
     * ###################################################
     * ############## WRITE ONLY METHODS #################
     * ###################################################
     */

    constexpr inline void serialize_null() const 
    noexcept(noexcept(__ctx__->backend_.write_null()))
    { 
        static_assert(IsWriting, "serialize_null is only callable in Write/Serialize mode!");
        __ctx__->backend_.write_null();
    }

    /**
     * ###################################################
     * ############## READ ONLY METHODS ##################
     * ###################################################
     */

    constexpr inline bool is_null() const 
    noexcept(noexcept(__ctx__->backend_.read_is_null()))
    {
        static_assert(IsReading, "is_null is only callable in Read/Deserialize mode!");
        return __ctx__->backend_.read_is_null();
    }

    constexpr inline void skip() const
    noexcept(noexcept(__ctx__->backend_.read_skip()))
    {
        static_assert(IsReading, "skip is only callable in Read/Deserialize mode!");
        return __ctx__->backend_.read_skip(); 
    }

    constexpr inline bool has_next() const
    noexcept(noexcept(__ctx__->backend_.read_has_next()))
    {
        static_assert(IsReading, "has_next is only callable in Read/Deserialize mode!");
        return __ctx__->backend_.read_has_next();
    }

    constexpr inline std::string_view key() const
    noexcept(noexcept(__ctx__->backend_.read_key()))
    {
        static_assert(IsReading, "key(void) is only callable in Read/Deserialize mode!");
        return __ctx__->backend_.read_key();
    }
};

}

#endif