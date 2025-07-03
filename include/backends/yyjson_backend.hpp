#ifndef REFLEXX_YYJSON_BACKEND_HPP
#define REFLEXX_YYJSON_BACKEND_HPP

#include <cstddef>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cassert>
#include <span>

#include <yyjson.h>

namespace reflexx {
namespace backends {

struct YyjsonBackend
{
private:
    struct yyjson_read_ctx
    {
        yyjson_val* val;
        yyjson_arr_iter arrIter;
        bool isArr = false;
        bool hasNext = false;
    };

public:
    yyjson_mut_doc* write_doc = nullptr;
    yyjson_doc* read_doc = nullptr;
    const char* write_result = nullptr;
    std::size_t write_result_size = 0;

    // to track current container (array/object)
    std::vector<yyjson_mut_val*> write_stack;
    std::vector<yyjson_read_ctx> read_stack;

    bool reading = false;

    explicit YyjsonBackend()
    {
        write_doc = yyjson_mut_doc_new(nullptr);
        write_stack.push_back(nullptr);
        reading = false;
    }
    
    explicit YyjsonBackend(std::span<const char> input)
    {
        read_doc = yyjson_read(input.data(), input.size_bytes(), YYJSON_READ_ALLOW_COMMENTS);
        read_stack.push_back({ yyjson_doc_get_root(read_doc) });
        reading = true;
    }

    ~YyjsonBackend()
    {
        if (write_doc)      yyjson_mut_doc_free(write_doc);
        if (read_doc)       yyjson_doc_free(read_doc);
        if (write_result)   free((void*)write_result);
    }

    bool is_reading() const
    {
        return reading;
    }

    std::string_view get()
    {
        if (write_result == nullptr)
        {
            write_result = yyjson_mut_write(write_doc, YYJSON_WRITE_PRETTY, &write_result_size);
        }

        return { write_result, write_result_size };
    }

    /*
    *   ###################################
    *   ########### WRITING ###############
    *   ###################################
    */

    void write_key(std::string_view sv)
    {
        key_buffer = sv;
    }
    
    void write_begin_array()
    {
        auto arr = yyjson_mut_arr(write_doc);
        if (!write_stack.back()) {
            write_stack.back() = arr; // root
            yyjson_mut_doc_set_root(write_doc, arr);
        } else {
            append_value(arr);
        }
        write_stack.push_back(arr);
    }

    void write_end_array()
    {
        write_stack.pop_back();
    }
    
    void write_begin_object()
    {
        auto obj = yyjson_mut_obj(write_doc);
        if (!write_stack.back())
        {
            write_stack.back() = obj; // root
            yyjson_mut_doc_set_root(write_doc, obj);
        } 
        else
        {
            append_value(obj);
        }
        write_stack.push_back(obj);
    }

    void write_end_object()
    {
        write_stack.pop_back();
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    void write_number(T val)
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

    void write_bool(bool b)
    {
        append_value(yyjson_mut_bool(write_doc, b));
    }

    void write_string(std::string_view sv)
    {
        append_value(yyjson_mut_strn(write_doc, sv.data(), sv.size()));
    }
    
    void write_null()
    {
        append_value(yyjson_mut_null(write_doc));
    }

    /*
    *   ###################################
    *   ########### READING ###############
    *   ###################################
    */

    void read_key(std::string_view key)
    {
        read_stack.push_back({ yyjson_obj_getn(read_current().val, key.data(), key.size()) });
    }
    
    void read_begin_array()
    {
        auto& current = read_current();
        yyjson_arr_iter_init(current.val, &current.arrIter);
        current.isArr = true;
        current.hasNext = yyjson_arr_iter_has_next(&current.arrIter);

        // Add first element (or nullptr) to stack
        read_stack.push_back({ yyjson_arr_iter_next(&current.arrIter) });
    }
    
    void read_end_array()
    {
        // Pop arr element (or nullptr)
        read_stack.pop_back();

        // Pop arr (and add next element if in another array)
        arr_elem_read_pop();
    }
    
    void read_begin_object()
    {
        // Nothing to write
    }

    void read_end_object()
    {
        arr_elem_read_pop();
    }

    template <typename T>
    requires std::is_arithmetic_v<T>
    void read_number(T& val)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            val = static_cast<T>(yyjson_get_real(read_current().val));
        } 
        else if constexpr (std::is_signed_v<T>)
        {
            val = static_cast<T>(yyjson_get_sint(read_current().val));
        }
        else
        {
            val = static_cast<T>(yyjson_get_uint(read_current().val));
        }

        arr_elem_read_pop();
    }

    void read_bool(bool& b)
    {
        b = yyjson_get_bool(read_current().val);

        arr_elem_read_pop();
    }

    std::string_view read_string()
    {
        std::string_view view = { yyjson_get_str(read_current().val), yyjson_get_len(read_current().val) };

        arr_elem_read_pop();

        return view;
    }

    bool read_is_null()
    {
        return yyjson_is_null(read_current().val);
    }

    void read_skip()
    {
        arr_elem_read_pop();
    }

    bool read_has_next()
    {
        return read_stack.size() > 1 && read_stack[read_stack.size() - 2].hasNext;
    }

private:
    std::string_view key_buffer;
    
    // Use this after writing is done
    yyjson_mut_val* write_root()
    {
        return write_stack.front();
    }

    yyjson_read_ctx& read_root()
    {
        return read_stack.front();
    }

    yyjson_mut_val* write_current()
    {
        return write_stack.back();
    }

    yyjson_read_ctx& read_current()
    {
        return read_stack.back();
    }
    
    void arr_elem_read_pop()
    {
        read_stack.pop_back();
        if (!read_stack.empty() && read_current().isArr)
        {
            read_current().hasNext = yyjson_arr_iter_has_next(&read_current().arrIter);
            read_stack.push_back({ yyjson_arr_iter_next(&read_current().arrIter) });
        }
    }

    void append_value(yyjson_mut_val* val)
    {
        yyjson_mut_val* parent = write_current();

        if (yyjson_mut_is_arr(parent))
        {
            yyjson_mut_arr_add_val(parent, val);
        } 
        else if (yyjson_mut_is_obj(parent))
        {
            assert(!key_buffer.empty());
            yyjson_mut_obj_add_val(write_doc, parent, key_buffer.data(), val);
            key_buffer = {};
        } else
        {
            assert(false && "Parent must be array or object.");
        }
    }
};

} // backends
} // reflexx

#endif