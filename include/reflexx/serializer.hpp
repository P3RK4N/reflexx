#ifndef REFLEXX_SERIALIZER_HPP
#define REFLEXX_SERIALIZER_HPP

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "reflexx/declare.hpp"

#include "reflexx/type_handler.hpp"
#include "reflexx/type_handler_list.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/util/enum_conv.hpp"
#include "reflexx/builtin/handlers/default_handler.hpp"
#include "reflexx/builtin/handlers/std_handler.hpp"
#include "reflexx/provider.hpp"
#include "reflexx/util/serializable.hpp"

namespace reflexx
{

using namespace util;

template <
    serializer_settings SerializerSettings,
    IsBackendType BackendType,
    IsTypeHandlerList TypeHandlerList = type_handler_list<std_handler, default_handler>
>
class serializer final
{
    template <IsSerializer TSerializer, bool IsReading>
    friend class type_handler;

    // TODO: Test access to this
    template <bool IsReading>
    struct serializer_context final
    {
        using handler_tuple_type = typename TypeHandlerList::template handler_tuple_t<serializer, IsReading>;
        using handler_base_type = type_handler<serializer, IsReading>;

        friend serializer;
        friend handler_base_type;

        serializer_context() requires (!IsReading)
        : backend_(), handlers_()
        {
            init_handlers();
        }

        serializer_context(std::string_view text) requires IsReading
        : backend_({ text }), handlers_()
        {
            init_handlers();
        }

        serializer_context(serializer_context&&)                 = default;
        serializer_context& operator=(serializer_context&&)      = default;
        serializer_context(const serializer_context&)            = delete;
        serializer_context& operator=(const serializer_context&) = delete;

        inline constexpr void init_handlers() noexcept
        {
            template for (constexpr auto I : util::enumerate<std::tuple_size_v<handler_tuple_type>>)
            {
                using handler_t = std::tuple_element_t<I, handler_tuple_type>;

                static_assert(std::derived_from<handler_t, handler_base_type>, "Handler should derive from type_handler with forwarded template params!");
                static_assert(std::is_default_constructible_v<handler_t>, "Handler should be nothrow default constructible!");
                static_assert(std::is_nothrow_destructible_v<handler_t>, "Handler should be nothrow destructible!");
                static_assert(std::is_move_constructible_v<handler_t>, "Handler should be move constructible!");
                static_assert(std::is_move_assignable_v<handler_t>, "Handler should be move constructible!");

                static_cast<handler_base_type&>(std::get<I>(handlers_)).__ctx__ = this;
            }
        }

        BackendType         backend_ {};
        handler_tuple_type  handlers_ {};

        template <typename T>
        inline constexpr auto& handler_for() noexcept
        {
            return std::get<handler_list_type::template get_first_index_v<serializer, IsReading, T, util::use_exact_handler_matching_v<SerializerSettings>>>(handlers_);
        }
    };

/*
    #########################################################################
    ############################# Interface #################################    
    #########################################################################
*/
public:
    static constexpr serializer_settings    settings            = SerializerSettings;
    static constexpr bool                   Read                = true;
    static constexpr bool                   Write               = !Read;
    
    using                                   handler_list_type   = TypeHandlerList;
    using                                   backend_type        = BackendType;
    using                                   read_context        = serializer_context<Read>;
    using                                   write_context       = serializer_context<Write>;
    using                                   result_context      = std::variant<read_context, write_context>;


    friend result_context;

    // TODO: Test access
    // TODO: Merge with context, there is no need for 2 internal structs, or is there?
    template <typename T>
    class result
    {
        friend serializer;

    public:
        inline constexpr T& get() noexcept
        {
            return res_;
        }

        inline constexpr const T& get() const noexcept
        {
            return res_;
        }

        inline constexpr T& operator*() noexcept
        {
            return res_;
        }

        inline constexpr const T& operator*() const noexcept
        {
            return res_;
        }
        
    private:
        // In-place read
        result(T res, std::string_view input) requires std::is_reference_v<T>
        : res_(res), ctx_(std::in_place_type<read_context>, input) {}
        
        // Return val read
        result(std::string_view input)
        : res_(provider<T>{}()), ctx_(std::in_place_type<read_context>, input) {}

        // Return val write
        result()
        : res_(), ctx_(std::in_place_type<write_context>) {}

        template <bool IsReading>
        auto& context()
        {
            if constexpr (IsReading)
            {
                return std::get<read_context>(ctx_);
            }
            else
            {
                return std::get<write_context>(ctx_);
            }
        }

        T res_;
        result_context ctx_;
    };

    template <util::is_serializable_type T>
    static inline constexpr result<std::string_view> serialize(const T& obj)
    {
        auto res = result<std::string_view>{};
        auto& ctx = res.template context<Write>();
        serialize(obj, ctx);
        *res = ctx.backend_.get();
        return res;
    };

    template <util::is_serializable_type T>
    static inline constexpr result<T&> deserialize(T& obj, std::string_view text)
    {
        auto res = result<T&>(obj, text);
        deserialize(*res, res.template context<Read>());
        return res;
    }

    template <util::is_serializable_type T>
    static inline constexpr result<T> deserialize(std::string_view text)
    {
        auto res = result<T>(text);
        deserialize(*res, res.template context<Read>());
        return res;
    }

/*
    #########################################################################
    ############################## Internal #################################    
    #########################################################################
*/

private:
    serializer()                                = delete;
    serializer(const serializer&)               = delete;
    serializer(serializer&&)                    = delete;
    serializer& operator=(const serializer&)    = delete;
    serializer& operator=(serializer&&)         = delete;

/*
    #########################################################################
    ######################### Type serializers ##############################    
    #########################################################################
*/

    template <util::is_serializable_class T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.template handler_for<std::remove_cvref_t<T>>().serialize(const_cast<T&>(obj))))
    {
        // NOTE: We need to cast constness away due to symmetric read/write api.
        //  Method signature promises constness and that is what reflexx will ensure.
        // TODO: Prevent handlers from modifying during Write mode
        ctx.template handler_for<std::remove_cvref_t<T>>().serialize(const_cast<T&>(obj));
    }

    template <util::is_serializable_class T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.template handler_for<std::remove_cvref_t<T>>().serialize(obj)))
    {
        ctx.template handler_for<std::remove_cvref_t<T>>().serialize(obj);
    }

    template <util::is_serializable_array T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(
        noexcept(ctx.backend_.write_begin_array()) &&
        noexcept(serialize(obj[0], ctx)) &&
        noexcept(ctx.backend_.write_end_array())
    )
    {
        using TArray = std::remove_cvref_t<T>;

        ctx.backend_.write_begin_array();

        for (std::size_t i = 0; i < std::extent_v<TArray>; i++)
        {
            serialize(obj[i], ctx);
        }

        ctx.backend_.write_end_array();
    }

    template <util::is_serializable_array T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(
        noexcept(ctx.backend_.read_begin_array()) &&
        noexcept(deserialize(obj[0], ctx)) &&
        noexcept(ctx.backend_.read_end_array())
    )
    {
        using TArray = std::remove_cvref_t<T>;

        ctx.backend_.read_begin_array();

        for (std::size_t i = 0; i < std::extent_v<TArray>; i++)
        {
            deserialize(obj[i], ctx);
        }

        ctx.backend_.read_end_array();
    }

    template <util::is_serializable_nullptr T>
    static inline constexpr void serialize(const T&, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_null()))
    {
        ctx.backend_.write_null();
    }

    template <util::is_serializable_nullptr T>
    static inline constexpr void deserialize(T&, read_context& ctx)
    noexcept(noexcept(ctx.backend_.read_skip()))
    {
        ctx.backend_.read_skip();
    }

    template <util::is_serializable_bool T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_bool(obj)))
    {
        ctx.backend_.write_bool(obj);
    }

    template <util::is_serializable_bool T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_.read_bool(obj)))
    {
        ctx.backend_.read_bool(obj);
    }

    template <util::is_serializable_string T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_string(std::declval<std::string_view>())))
    {
        // NOTE: ASCII and UTF8 are handled the same way currently
        ctx.backend_.write_string({ reinterpret_cast<const char*>(obj.data()), obj.size() });
    }

    template <util::is_serializable_string T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(
        noexcept(ctx.backend_.read_string()) &&
        noexcept(obj = std::declval<std::basic_string_view<typename std::remove_cvref_t<T>::value_type>>())
    )
    {
        using CharT = typename std::remove_cvref_t<T>::value_type;
        auto view = ctx.backend_.read_string();
        obj = std::basic_string_view<CharT>{ reinterpret_cast<const CharT*>(view.data()), view.size() };
    }

    template <util::is_serializable_number T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_number(obj)))
    {
        ctx.backend_.write_number(obj);
    }

    template <util::is_serializable_number T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_.read_number(obj)))
    {
        ctx.backend_.read_number(obj);
    }

    template <util::is_serializable_char T>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_char(std::declval<char>())))
    {
        ctx.backend_.write_char(static_cast<char>(obj));
    }

    template <util::is_serializable_char T>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_.read_char(std::declval<char&>())))
    {
        // NOTE: Maybe UB
        ctx.backend_.read_char(reinterpret_cast<char&>(obj));
    }

    template <util::is_serializable_enum T> 
    requires format_enum_as_string_v<SerializerSettings>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_.write_string(std::declval<std::string_view>())))
    {
        ctx.backend_.write_string(util::enum_to_string(obj));
    }

    template <util::is_serializable_enum T>
    requires format_enum_as_string_v<SerializerSettings>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_.read_string()))
    {
        obj = util::string_to_enum<std::remove_cvref_t<T>>(ctx.backend_.read_string());
    }

    template <util::is_serializable_enum T>
    requires (!format_enum_as_string_v<SerializerSettings>)
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(serializer::serialize(static_cast<std::underlying_type_t<std::remove_cvref_t<T>>>(obj), ctx)))
    {
        serializer::serialize(static_cast<std::underlying_type_t<std::remove_cvref_t<T>>>(obj), ctx);
    }
        
    template <util::is_serializable_enum T>
    requires (!format_enum_as_string_v<SerializerSettings>)
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(serializer::deserialize(std::declval<std::underlying_type_t<std::remove_cvref_t<T>>&>(), ctx)))
    {
        // NOTE: Maybe UB
        serializer::deserialize(reinterpret_cast<std::underlying_type_t<std::remove_cvref_t<T>>&>(obj), ctx);
    }
};

}

#endif

