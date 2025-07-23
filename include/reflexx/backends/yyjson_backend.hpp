#ifndef REFLEXX_YYJSON_BACKEND_HPP
#define REFLEXX_YYJSON_BACKEND_HPP

#include <cstddef>
#include <string_view>
#include <variant>
#include <vector>
#include <cstdint>
#include <cassert>
#include <span>

#include <yyjson.h>

namespace reflexx {
namespace backends {

struct yyjson_backend
{
public:
    inline explicit yyjson_backend()
    {
        write_doc = yyjson_mut_doc_new(nullptr);
        write_stack.reserve(8);
    }
    
    inline explicit yyjson_backend(std::span<const char> input)
    {
        read_doc = yyjson_read(input.data(), input.size_bytes(), YYJSON_READ_ALLOW_COMMENTS);
        read_stack.reserve(8);

        val_cache = yyjson_doc_get_root(read_doc);
    }

    inline ~yyjson_backend()
    {
        if (write_doc)      yyjson_mut_doc_free(write_doc);
        if (read_doc)       yyjson_doc_free(read_doc);
        if (write_result)   free((void*)write_result);
    }

    inline std::string_view get()
    {
        write_result = write_result ? write_result : yyjson_mut_write(write_doc, YYJSON_WRITE_PRETTY, &write_result_size);

        return { write_result, write_result_size };
    }

    /*
    *   ###################################
    *   ########### WRITING ###############
    *   ###################################
    */

    inline void write_key(std::string_view sv)
    {
        key_cache = sv;
    }
    
    inline void write_begin_array()
    {
        auto arr = yyjson_mut_arr(write_doc);
        append_value(arr);
        write_stack.push_back(arr);
    }

    inline void write_end_array()
    {
        write_stack.pop_back();
    }
    
    inline void write_begin_object()
    {
        auto obj = yyjson_mut_obj(write_doc);
        append_value(obj);
        write_stack.push_back(obj);
    }

    inline void write_end_object()
    {
        write_stack.pop_back();
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void write_number(T val)
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

    inline void write_bool(bool b)
    {
        append_value(yyjson_mut_bool(write_doc, b));
    }

    inline void write_string(std::string_view sv)
    {
        append_value(yyjson_mut_strn(write_doc, sv.data(), sv.size()));
    }
    
    inline void write_null()
    {
        append_value(yyjson_mut_null(write_doc));
    }

    /*
    *   ###################################
    *   ########### READING ###############
    *   ###################################
    */

    inline void read_key(std::string_view key)
    {
        val_cache = yyjson_obj_getn(read_stack.back().val, key.data(), key.size());
    }

    inline std::string_view read_key()
    {
        auto key = yyjson_obj_iter_next(&std::get<yyjson_obj_iter>(read_stack.back().iter));
        val_cache = yyjson_obj_iter_get_val(key);
        return { unsafe_yyjson_get_str(key), unsafe_yyjson_get_len(key) };
    }
    
    inline void read_begin_array()
    {
        scoped_val_cacher _ { this };

        read_stack.push_back({ val_cache, yyjson_arr_iter_with(val_cache) });
    }
    
    inline void read_end_array()
    {
        read_stack.pop_back();
    }
    
    inline void read_begin_object()
    {
        scoped_val_cacher _ { this };

        read_stack.push_back({ val_cache, yyjson_obj_iter_with(val_cache) });
    }

    inline void read_end_object()
    {
        read_stack.pop_back();
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    inline void read_number(T& val)
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

    inline void read_bool(bool& b)
    {
        scoped_val_cacher _ { this };

        b = unsafe_yyjson_get_bool(val_cache);
    }

    inline std::string_view read_string()
    {
        scoped_val_cacher _ { this };

        return { unsafe_yyjson_get_str(val_cache), unsafe_yyjson_get_len(val_cache) };
    }

    inline bool read_is_null()
    {
        val_cache = val_cache ? val_cache : yyjson_arr_iter_next(&std::get<yyjson_arr_iter>(read_stack.back().iter));

        return unsafe_yyjson_is_null(val_cache);
    }

    inline void read_skip()
    {
        scoped_val_cacher _ { this };
    }

    inline bool read_has_next()
    {
        return unsafe_yyjson_is_arr(read_stack.back().val) ? 
            yyjson_arr_iter_has_next(&std::get<yyjson_arr_iter>(read_stack.back().iter)) :
            yyjson_obj_iter_has_next(&std::get<yyjson_obj_iter>(read_stack.back().iter));
    }

private:
    using reflexx_yyjson_iter = std::variant<yyjson_arr_iter, yyjson_obj_iter>;

    struct yyjson_read_ctx
    {
        yyjson_val* val;
        reflexx_yyjson_iter iter { yyjson_arr_iter{} };
    };

    /**
    * @brief RAII utility for managing cached values during array iteration.
    *
    * - On construction: preloads the next array element into the internal cache, if not already cached.
    * - On destruction: flushes the cached value to ensure consistency.
    * 
    * @note This helper is intended solely for array iterators.
    *       Iterating over objects or other types requires reading keys first,
    *       which automatically populates the cache.
    */
    struct scoped_val_cacher
    {
        yyjson_backend* _parent;

        inline scoped_val_cacher(yyjson_backend* parent)
        : _parent(parent)
        {
            // If nullptr, we are in array
            _parent->val_cache = _parent->val_cache ? _parent->val_cache : yyjson_arr_iter_next(&std::get<yyjson_arr_iter>(_parent->read_stack.back().iter));
        }

        inline ~scoped_val_cacher()
        {
            _parent->val_cache = nullptr;
        }
    };

    yyjson_mut_doc* write_doc = nullptr;
    yyjson_doc* read_doc = nullptr;
    const char* write_result = nullptr;
    std::size_t write_result_size = 0;

    // to track current container (array/object)
    std::vector<yyjson_mut_val*> write_stack;
    std::vector<yyjson_read_ctx> read_stack;

    std::string_view key_cache {};
    yyjson_val* val_cache {};

    inline void append_value(yyjson_mut_val* val)
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

} // backends
} // reflexx

#endif