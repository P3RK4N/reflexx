#ifndef REFLEXX_YYJSON_BACKEND_HPP
#define REFLEXX_YYJSON_BACKEND_HPP

#include <cstddef>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cassert>
#include <span>

#include <yyjson.h>

#include "reflexx/util/serializable.hpp"

namespace reflexx::backends {

struct yyjson_backend
{
public:
    inline explicit yyjson_backend()
    : write_doc(yyjson_mut_doc_new(nullptr))
    {
        write_stack.reserve(InitialVectorSize);
    }
    
    inline explicit yyjson_backend(std::span<char> mutable_input)
    : yyjson_backend(static_cast<std::span<const char>>(mutable_input)) {}

    inline explicit yyjson_backend(std::span<const char> input)
    : read_doc(yyjson_read(input.data(), input.size_bytes(), YYJSON_READ_ALLOW_COMMENTS)), val_cache(yyjson_doc_get_root(read_doc))
    {
        read_stack.reserve(InitialVectorSize);
    }
    
    inline ~yyjson_backend() noexcept
    {
        if (write_doc)      yyjson_mut_doc_free(write_doc);
        if (read_doc)       yyjson_doc_free(read_doc);
        if (write_result)   free((void*)write_result);
    }

    inline std::string_view get() noexcept
    {
        write_result = write_result ? write_result : yyjson_mut_write(write_doc, YYJSON_WRITE_PRETTY, &write_result_size);

        return { write_result, write_result_size };
    }

    /*
    *   ###################################
    *   ########### WRITING ###############
    *   ###################################
    */

    inline void write_key(std::string_view sv) noexcept
    {
        key_cache = sv;
    }
    
    inline void write_begin_array()
    {
        auto arr = yyjson_mut_arr(write_doc);
        append_value(arr);
        write_stack.push_back(arr);
    }

    inline void write_end_array() noexcept
    {
        write_stack.pop_back();
    }
    
    inline void write_begin_object()
    {
        auto obj = yyjson_mut_obj(write_doc);
        append_value(obj);
        write_stack.push_back(obj);
    }

    inline void write_end_object() noexcept
    {
        write_stack.pop_back();
    }

    template <typename T>
    requires util::is_serializable_number_v<T>
    inline void write_number(const T val) noexcept
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            append_value(yyjson_mut_real(write_doc, static_cast<double>(val)));
        } 
        else if constexpr (std::is_signed_v<T>)
        {
            append_value(yyjson_mut_sint(write_doc, static_cast<int64_t>(val)));
        }
        else
        {
            append_value(yyjson_mut_uint(write_doc, static_cast<uint64_t>(val)));
        }
    }

    inline void write_char(const char c) noexcept
    {
        append_value(yyjson_mut_strn(write_doc, &c, 1));
    }

    inline void write_bool(const bool b) noexcept
    {
        append_value(yyjson_mut_bool(write_doc, b));
    }

    inline void write_string(std::string_view sv) noexcept
    {
        append_value(yyjson_mut_strn(write_doc, sv.data(), sv.size()));
    }
    
    inline void write_null() noexcept
    {
        append_value(yyjson_mut_null(write_doc));
    }

    /*
    *   ###################################
    *   ########### READING ###############
    *   ###################################
    */

    inline void read_key(std::string_view key) noexcept
    {
        val_cache = yyjson_obj_getn(read_stack.back().val, key.data(), key.size());
    }

    inline std::string_view read_key() noexcept
    {
        auto key = yyjson_obj_iter_next(&read_stack.back().iter.obj_iter);
        val_cache = yyjson_obj_iter_get_val(key);
        return { unsafe_yyjson_get_str(key), unsafe_yyjson_get_len(key) };
    }
    
    inline void read_begin_array()
    {
        scoped_val_cacher _ { this };

        read_stack.push_back({ val_cache, { .arr_iter = yyjson_arr_iter_with(val_cache) } });
    }
    
    inline void read_end_array() noexcept
    {
        read_stack.pop_back();
    }
    
    inline void read_begin_object()
    {
        scoped_val_cacher _ { this };

        read_stack.push_back({ val_cache, { .obj_iter = yyjson_obj_iter_with(val_cache) } });
    }

    inline void read_end_object() noexcept
    {
        read_stack.pop_back();
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void read_number(T& val) noexcept
    {
        scoped_val_cacher _ { this };

        if constexpr (std::is_floating_point_v<T>)
        {
            val = static_cast<T>(unsafe_yyjson_get_real(val_cache));
        } 
        else if constexpr (std::is_signed_v<T>)
        {
            val = static_cast<T>(unsafe_yyjson_get_sint(val_cache));
        }
        else
        {
            val = static_cast<T>(unsafe_yyjson_get_uint(val_cache));
        }
    }

    inline void read_char(char& c) noexcept
    {
        scoped_val_cacher _ { this };

        c = *unsafe_yyjson_get_str(val_cache);
    }

    inline void read_bool(bool& b) noexcept
    {
        scoped_val_cacher _ { this };

        b = unsafe_yyjson_get_bool(val_cache);
    }

    inline std::string_view read_string() noexcept
    {
        scoped_val_cacher _ { this };

        return { unsafe_yyjson_get_str(val_cache), unsafe_yyjson_get_len(val_cache) };
    }

    inline bool read_is_null() noexcept
    {
        val_cache = val_cache ? val_cache : yyjson_arr_iter_next(&read_stack.back().iter.arr_iter);

        return unsafe_yyjson_is_null(val_cache);
    }

    inline void read_skip() noexcept
    {
        scoped_val_cacher _ { this };
    }

    inline bool read_has_next() noexcept
    {
        return unsafe_yyjson_is_arr(read_stack.back().val) ?
            yyjson_arr_iter_has_next(&read_stack.back().iter.arr_iter) :
            yyjson_obj_iter_has_next(&read_stack.back().iter.obj_iter);
    }

private:
    union reflexx_yyjson_iter
    {
        yyjson_arr_iter arr_iter;
        yyjson_obj_iter obj_iter;
    };

    struct yyjson_read_ctx
    {
        yyjson_val* val;
        reflexx_yyjson_iter iter;
    };

    /**
    * @brief RAII utility for managing cached values during array iteration.
    *
    * - On construction: preloads the next array element into the internal cache, if not already cached.
    * - On destruction: flushes the cached value to ensure consistency.
    * 
    * @note This helper is intended solely for array iterators.
    *       Iterating over objects or other types requires reading keys first,
    *       which populate the val_cache, preventing arr iteration.
    */
    struct scoped_val_cacher
    {
        yyjson_backend* _parent;

        inline scoped_val_cacher(yyjson_backend* parent) noexcept
        : _parent(parent)
        {
            // If nullptr, we are in array
            _parent->val_cache = _parent->val_cache ? _parent->val_cache : yyjson_arr_iter_next(&_parent->read_stack.back().iter.arr_iter);
        }

        inline ~scoped_val_cacher() noexcept
        {
            _parent->val_cache = nullptr;
        }
    };

    static constexpr size_t InitialVectorSize = 8;

    yyjson_mut_doc* write_doc = nullptr;
    yyjson_doc* read_doc = nullptr;
    const char* write_result = nullptr;
    std::size_t write_result_size = 0;

    // to track current container (array/object)
    std::vector<yyjson_mut_val*> write_stack;
    std::vector<yyjson_read_ctx> read_stack;

    std::string_view key_cache {};
    yyjson_val* val_cache {};

    inline void append_value(yyjson_mut_val* val) noexcept
    {
        // No parent
        if (write_stack.empty())
        {
            yyjson_mut_doc_set_root(write_doc, val);
        }
        // Array parent
        else if (auto& parent = write_stack.back(); unsafe_yyjson_is_arr(parent))
        {
            yyjson_mut_arr_add_val(parent, val);
        }
        // Object parent
        else
        {
            yyjson_mut_obj_add_val(write_doc, parent, key_cache.data(), val);
        }
    }
};

} // reflexx::backends

#endif