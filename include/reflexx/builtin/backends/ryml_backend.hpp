#ifndef REFLEXX_RYML_BACKEND_HPP
#define REFLEXX_RYML_BACKEND_HPP

#include <cstddef>
#include <string_view>
#include <span>
#include <cassert>
#include <string>

#include <ryml.hpp>
#include <c4/std/std.hpp>
#include <c4/substr_fwd.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/fwd.hpp>
#include <c4/yml/node.hpp>
#include <c4/yml/node_type.hpp>
#include <type_traits>

#include "c4/yml/parse.hpp"
#include "reflexx/util/serializable_number.hpp"

// TODO: Check if noexcept is applicable here

namespace reflexx::backends {

struct ryml_backend
{

public:
    inline explicit ryml_backend()
    : tree(InitialArenaSize), current_node(tree.rootref())
    {
        
    }

    inline explicit ryml_backend(std::span<char> input)
    : tree(c4::yml::parse_in_place(c4::substr(input.data(), input.size()))), current_node(tree.rootref())
    {
        read_stack.reserve(InitialVectorSize);
    }

    inline explicit ryml_backend(std::span<const char> input)
    : tree(c4::yml::parse_in_arena(c4::csubstr(input.data(), input.size()))), current_node(tree.rootref())
    {
        read_stack.reserve(InitialVectorSize);
    }

    inline std::string_view get()
    {
        write_result = write_result.empty() ? std::move(emitrs_yaml<std::string>(tree)) : write_result;
        return write_result;
    }

    /*
    *   ###################################
    *   ########### WRITING ###############
    *   ###################################
    */

    inline void write_key(std::string_view sv) noexcept
    {
        c4::csubstr key_view = to_csubstr(sv);
        current_node = current_node.append_child() << c4::yml::key(key_view);
    }

    inline void write_begin_array()
    {
        current_node = current_node.is_seq() ? current_node.append_child() : current_node;
        current_node |= c4::yml::SEQ;
    }

    inline void write_end_array() noexcept
    {
        current_node = current_node.parent();
    }

    inline void write_begin_object()
    {
        current_node = current_node.is_seq() ? current_node.append_child() : current_node;
        current_node |= c4::yml::MAP;
    }

    inline void write_end_object() noexcept
    {
        current_node = current_node.parent();
    }

    template <typename T>
    requires util::is_serializable_number_v<T>
    inline void write_number(const T num)
    {
        append_value(num);
    }

    inline void write_char(char c)
    {
        append_value(c);
    }

    inline void write_bool(bool b)
    {
        append_value(b);
    }

    inline void write_string(std::string_view sv)
    {
        append_value(to_csubstr(sv));
    }

    inline void write_null()
    {
        append_value(nullptr);
    }

    /*
    *   ###################################
    *   ########### READING ###############
    *   ###################################
    */

    inline void read_key(std::string_view key)
    {
        current_node = read_stack.back().val[to_csubstr(key)];
    }

    inline std::string_view read_key()
    {
        current_node = *read_stack.back().iter;
        ++read_stack.back().iter;
        return to_string_view(current_node.key());
    }
    
    inline void read_begin_array()
    {
        scoped_node_cacher _ { this };

        read_stack.push_back({ .val = current_node, .iter = current_node.begin() });
    }

    inline constexpr void read_end_array()
    {
        read_stack.pop_back();
    }

    inline constexpr void read_begin_object()
    {
        scoped_node_cacher _ { this };

        read_stack.push_back({ .val = current_node, .iter = current_node.begin() });
    }

    inline constexpr void read_end_object()
    {
        read_stack.pop_back();
    }

    template <typename T>
    requires util::is_serializable_number_v<T>
    inline void read_number(T& num)
    {
        read_value(num);
    }

    inline void read_char(char& c)
    {
        read_value(c);
    }

    inline void read_bool(bool& b)
    {
        read_value(b);
    }

    inline std::string_view read_string()
    {
        c4::csubstr view {};
        read_value(view);
        return to_string_view(view);
    }

    inline bool read_is_null()
    {
        if (current_node.invalid())
        {
            current_node = *read_stack.back().iter;
            ++read_stack.back().iter;
        }

        return !current_node.type().is_container() && (!current_node.type().has_val() || current_node.type().val_is_null());
    }

    inline void read_skip()
    {
        scoped_node_cacher _ { this };
    }

    inline bool read_has_next()
    {
        return read_stack.back().iter != read_stack.back().val.end();
    }

private:
    struct ryml_read_ctx
    {
        c4::yml::NodeRef val;
        c4::yml::NodeRef::iterator iter;
    };

    /**
    * @brief RAII utility for managing current node during array iteration.
    *
    * - On construction: preloads the next array element into the current_node, if not already present.
    * - On destruction: flushes the current_node to ensure consistency.
    * 
    * @note This helper is intended solely for array iterators.
    *       Iterating over objects or other types requires reading keys first,
    *       which populate the current_node, preventing arr iteration.
    */
    struct scoped_node_cacher
    {
        ryml_backend* _parent;

        inline scoped_node_cacher(ryml_backend* parent) noexcept
        : _parent(parent)
        {
            // If invalid, we are in array
            if (_parent->current_node.invalid())
            {
                _parent->current_node = *_parent->read_stack.back().iter;
                ++_parent->read_stack.back().iter;
            }
        }

        inline ~scoped_node_cacher() noexcept
        {
            _parent->current_node = c4::yml::NodeRef{};
        }
    };

    static constexpr size_t InitialVectorSize = 8;
    static constexpr size_t InitialArenaSize = 64;

    c4::yml::Tree tree;

    c4::yml::NodeRef current_node;
    std::string write_result;
    
    std::vector<ryml_read_ctx> read_stack;

    template <typename T>
    inline void append_value(T&& val)
    {
        current_node = current_node.is_seq() ? current_node.append_child() : current_node;
        current_node << val;
        current_node = current_node.parent();
    }

    template <typename T>
    inline void read_value(T& val)
    {
        scoped_node_cacher _ { this };

        current_node >> val;
    }

    template <typename T>
    requires std::is_same_v<c4::csubstr, T> || std::is_same_v<c4::substr, T>
    inline std::string_view to_string_view(const T& view)
    {
        return { view.data(), view.size() };
    }

    inline c4::csubstr to_csubstr(const std::string_view& view)
    {
        return { view.data(), view.size() };
    }
};

} // reflexx::backends

#endif