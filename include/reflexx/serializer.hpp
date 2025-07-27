#ifndef REFLEXX_SERIALIZER_HPP
#define REFLEXX_SERIALIZER_HPP

#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "reflexx/declare.hpp"

#include "reflexx/type_handler.hpp"
#include "reflexx/type_handler_list.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/util/non_serializable_category_type.hpp"
#include "reflexx/util/enum_conv.hpp"
#include "reflexx/builtin/default_handler.hpp"
#include "reflexx/builtin/std_handler.hpp"
#include "reflexx/provider.hpp"
#include "reflexx/util/serializable_number.hpp"

namespace reflexx
{

template <
    serializer_settings SerializerSettings,
    IsBackendType BackendType,
    IsTypeHandlerList TypeHandlerList = type_handler_list<std_handler, default_handler>
>
class serializer final
{
    template <IsSerializer TSerializer, bool IsReading>
    friend class type_handler;

/*
    #########################################################################
    ############################# Interface #################################    
    #########################################################################
*/
public:
    class serializer_result final
    {    
        friend class serializer;
        
    private:
        inline constexpr serializer_result(std::unique_ptr<BackendType>&& backend) 
        : backend_(std::move(backend)) {}

        std::unique_ptr<BackendType> backend_ {};

    public:
        inline constexpr std::string_view get() const noexcept
        {
            return backend_->get();
        }

        inline constexpr std::string_view operator*() const noexcept
        {
            return this->get();
        }
    };

    template <typename T>
    static inline constexpr serializer_result serialize(const T& obj)
    {
        write_context ctx {};
        serialize(obj, ctx);

        return { std::move(ctx.backend_) };
    };

    template <typename T>
    static inline constexpr void deserialize(T& obj, std::string_view text)
    {
        read_context ctx { text };
        deserialize(obj, ctx);
    }

    template <typename T>
    requires std::is_default_constructible_v<T>
    static inline constexpr T deserialize(std::string_view text)
    {
        T t = provider<T>{}();
        deserialize(t, text);

        return t;
    }

    static constexpr serializer_settings settings   = SerializerSettings;
    using handler_list_type                         = TypeHandlerList;
    using backend_type                              = BackendType;

/*
    #########################################################################
    ############################## Internal #################################    
    #########################################################################
*/

private:
    template <typename T>
    static inline constexpr void static_assert_category_type() noexcept
    {
        static_assert
        (
            !::reflexx::util::is_non_serializable_category_type_v<T>,
            "\n\n\n"
            "##########################################################\n"
            "################## ˇSERIALIZATION ERRORˇ #################\n"
            "##########################################################\n"
            "\n\n"
            "Non owning (raw) pointers, references, member pointers,\n" 
            "unbounded arrays, functions and unions are not supported\n"
            "by default. Consider using smart pointers, vectors and variants!\n"
            "You can also use custom serializers to handle these types.\n"
            "\n\n"
            "##########################################################\n"
            "################## ^SERIALIZATION ERROR^ #################\n"
            "##########################################################\n"
            "\n\n\n"
        );
    }

private:
    serializer()                                = delete;
    serializer(const serializer&)               = delete;
    serializer(serializer&&)                    = delete;
    serializer& operator=(const serializer&)    = delete;
    serializer& operator=(serializer&&)         = delete;

    template <bool IsReading>
    struct serializer_context final
    {
        using base_type = type_handler<serializer, IsReading>;
        using handler_tuple_t = typename TypeHandlerList::template handler_tuple_t<serializer, IsReading>;

        serializer_context() requires (!IsReading)
        : backend_(std::make_unique<BackendType>()), handlers_()
        {
            init_handlers();
        }

        serializer_context(std::string_view text) requires IsReading
        : backend_(std::make_unique<BackendType>(text)), handlers_()
        {
            init_handlers();
        }
        
        serializer_context(const serializer_context&)            = delete;
        serializer_context(serializer_context&&)                 = delete;
        serializer_context& operator=(const serializer_context&) = delete;
        serializer_context& operator=(serializer_context&&)      = delete;

        inline constexpr void init_handlers() noexcept
        {
            template for (constexpr auto I : util::enumerate<std::tuple_size_v<handler_tuple_t>>())
            {
                using handler_t = std::tuple_element_t<I, handler_tuple_t>;
    
                static_assert(std::derived_from<handler_t, base_type>, "Handler should derive from type_handler with forwarded template params!");
                static_assert(std::is_nothrow_default_constructible_v<handler_t>, "Handler should be nothrow default constructible!");
    
                static_cast<base_type&>(std::get<I>(handlers_)).__ctx__ = this;
            }
        }

        std::unique_ptr<BackendType> backend_ {};
        handler_tuple_t handlers_ {};

        template <typename T>
        inline constexpr auto& handler_for() noexcept
        {
            return std::get<TypeHandlerList::template get_first_index_v<serializer, IsReading, T>>(handlers_);
        }
    };

    using read_context = serializer_context<true>;
    using write_context = serializer_context<false>;

/*
    #########################################################################
    ################### Type categories serializers #########################    
    #########################################################################
*/

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.template handler_for<std::remove_cvref_t<T>>().serialize(const_cast<T&>(obj))))
    {
        // NOTE: We need to cast constness away due to symmetric read/write api.
        //  Method signature promises constness and that is what user will get at the end. (I promise)
        ctx.template handler_for<std::remove_cvref_t<T>>().serialize(const_cast<T&>(obj));
    }

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.template handler_for<std::remove_cvref_t<T>>().serialize(obj)))
    {
        ctx.template handler_for<std::remove_cvref_t<T>>().serialize(obj);
    }

    template <typename T>
    requires std::is_bounded_array_v<std::remove_cvref_t<T>>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(
        noexcept(ctx.backend_->write_begin_array()) &&
        noexcept(serialize(obj[0], ctx)) &&
        noexcept(ctx.backend_->write_end_array())
    )
    {
        using TArray = std::remove_cvref_t<T>;

        ctx.backend_->write_begin_array();

        for (std::size_t i = 0; i < std::extent_v<TArray>; i++)
        {
            serialize(obj[i], ctx);
        }

        ctx.backend_->write_end_array();
    }

    template <typename T>
    requires std::is_bounded_array_v<std::remove_cvref_t<T>>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(
        noexcept(ctx.backend_->read_begin_array()) &&
        noexcept(deserialize(obj[0], ctx)) &&
        noexcept(ctx.backend_->read_end_array())
    )
    {
        using TArray = std::remove_cvref_t<T>;

        ctx.backend_->read_begin_array();

        for (std::size_t i = 0; i < std::extent_v<TArray>; i++)
        {
            deserialize(obj[i], ctx);
        }

        ctx.backend_->read_end_array();
    }

    template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, std::nullptr_t>
    static inline constexpr void serialize(const T&, write_context& ctx)
    noexcept(noexcept(ctx.backend_->write_null()))
    {
        ctx.backend_->write_null();
    }

    template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, std::nullptr_t>
    static inline constexpr void deserialize(T&, read_context& ctx)
    noexcept(noexcept(ctx.backend_->read_skip()))
    {
        ctx.backend_->read_skip();
    }

    template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, bool>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_->write_bool(obj)))
    {
        ctx.backend_->write_bool(obj);
    }

    template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, bool>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_->read_bool(obj)))
    {
        ctx.backend_->read_bool(obj);
    }

    template <typename T>
    requires util::is_serializable_number_v<std::remove_cvref_t<T>>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_->write_number(obj)))
    {
        ctx.backend_->write_number(obj);
    }

    template <typename T>
    requires util::is_serializable_number_v<std::remove_cvref_t<T>>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_->read_number(obj)))
    {
        ctx.backend_->read_number(obj);
    }

    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>> && format_enum_as_string_v<SerializerSettings>
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    {
        ctx.backend_->write_string(util::enum_to_string(obj));
    }

    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>> && format_enum_as_string_v<SerializerSettings>
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    {
        obj = util::string_to_enum<std::remove_cvref_t<T>>(ctx.backend_->read_string());
    }

    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>> && (!format_enum_as_string_v<SerializerSettings>)
    static inline constexpr void serialize(const T& obj, write_context& ctx)
    noexcept(noexcept(ctx.backend_->write_number(static_cast<std::underlying_type_t<std::remove_cvref_t<T>>>(obj))))
    {
        ctx.backend_->write_number(static_cast<std::underlying_type_t<std::remove_cvref_t<T>>>(obj));
    }
        
    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>> && (!format_enum_as_string_v<SerializerSettings>)
    static inline constexpr void deserialize(T& obj, read_context& ctx)
    noexcept(noexcept(ctx.backend_->read_number(std::declval<std::underlying_type_t<std::remove_cvref_t<T>>&>())))
    {
        std::underlying_type_t<std::remove_cvref_t<T>> value;
        ctx.backend_->read_number(value);
        obj = static_cast<std::remove_cvref_t<T>>(value);
    }

    template <typename T>
    requires util::is_non_serializable_category_type_v<std::remove_cvref_t<T>>
    static inline constexpr void serialize(const T& obj, write_context& ctx) noexcept
    {
        static_assert_category_type<std::remove_cvref_t<T>>();
    }

    template <typename T>
    requires util::is_non_serializable_category_type_v<std::remove_cvref_t<T>>
    static inline constexpr void deserialize(T& obj, read_context& ctx) noexcept
    {
        static_assert_category_type<std::remove_cvref_t<T>>();
    }
};

}

#endif

