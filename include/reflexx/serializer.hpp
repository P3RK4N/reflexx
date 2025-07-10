#ifndef REFLEXX_SERIALIZER_HPP
#define REFLEXX_SERIALIZER_HPP

#include <cstddef>
#include <memory>
#include <string_view>
#include <type_traits>

#include "reflexx/declare.hpp"

#include "reflexx/type_handler_list.hpp"
#include "reflexx/serializer_settings.hpp"
#include "reflexx/util/non_serializable_category_type.hpp"
#include "reflexx/util/enum_conv.hpp"
#include "reflexx/builtin/default_handler.hpp"
#include "reflexx/builtin/std_handler.hpp"

namespace reflexx
{

#define REFLEXX_INLINE_CONSTEXPR inline

template <
    serializer_settings SerializerSettings,
    IsBackendType BackendType,
    IsTypeHandlerList TypeHandlerList = type_handler_list<std_handler, default_handler>
>
class serializer final
{
    template <IsSerializer TSerializer, bool IsReading>
    friend class type_handler;

    template <typename T>
    static constexpr bool is_serializable_v = 
        std::is_class_v<std::remove_cvref_t<T>> || 
        std::is_unbounded_array_v<std::remove_cvref_t<T>>;

/*
    #########################################################################
    ############################# Interface #################################    
    #########################################################################
*/
public:
    class serializer_result final
    {    
    private:
        friend class serializer;
        inline constexpr serializer_result() = default;

        std::unique_ptr<BackendType> backend_ = nullptr;

    public:
        inline constexpr std::string_view get() const
        {
            return backend_->get();
        }

        inline constexpr std::string_view operator*() const
        {
            return this->get();
        }
    };

    template <typename T>
    requires is_serializable_v<T>
    static REFLEXX_INLINE_CONSTEXPR serializer_result serialize(const T& obj)
    {
        serializer_context ctx {};
        serializer_result result {};
        
        ctx.backend_ = std::make_unique<BackendType>();
        
        serialize(obj, ctx);
        
        result.backend_ = std::move(ctx.backend_);

        return result;
    };

    template <typename T>
    requires is_serializable_v<T>
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, std::string_view text)
    {
        serializer_context ctx {};
        
        ctx.backend_ = std::make_unique<BackendType>(text);
        
        deserialize(obj, ctx);
    }

    template <typename T>
    requires is_serializable_v<T> && std::is_default_constructible_v<T> // TODO: || IsProvideable<T>
    static REFLEXX_INLINE_CONSTEXPR T deserialize(std::string_view text)
    {
        T t {};
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
    static void static_assert_category_type()
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

    template <typename THandler, bool IsReading>
    static void static_assert_type_handler()
    {
        using THandlerBase = type_handler<serializer, IsReading>;

        static_assert
        (
            std::derived_from<THandler, THandlerBase> && std::is_default_constructible_v<THandler>, 
            "\n\n\n"
            "##########################################################\n"
            "################## ˇSERIALIZATION ERRORˇ #################\n"
            "##########################################################\n"
            "\n\n"
            "Handler should derive from type_handler with forwarded\n"
            "template params and be default constructible!\n"
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

    struct serializer_context final
    {
        std::unique_ptr<BackendType> backend_ {};
    };

/*
    #########################################################################
    ################### Type categories serializers #########################    
    #########################################################################
*/

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void serialize(const T& obj, serializer_context& ctx)
    {
        using THandler = typename TypeHandlerList::template get_first_t<serializer, /* IsReading */ false, T>;
        static_assert_type_handler<THandler, /* IsReading */ false>();

        // NOTE: We need to cast constness away due to symmetric read/write api.
        //  Method signature promises constness and that is what user will get at the end. (I promise)
        auto handler = THandler{};
        handler.__ctx__ = &ctx;
        handler.serialize(const_cast<T&>(obj));
    }

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, serializer_context& ctx)
    {
        using THandler = typename TypeHandlerList::template get_first_t<serializer, /* IsReading */ true, T>;
        static_assert_type_handler<THandler, /* IsReading */ true>();

        auto handler = THandler{};
        handler.__ctx__ = &ctx;
        handler.serialize(obj);
    }

    template <typename T>
    requires std::is_bounded_array_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void serialize(const T& obj, serializer_context& ctx)
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
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, serializer_context& ctx)
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
    requires std::is_fundamental_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void serialize(const T& obj, serializer_context& ctx)
    {
        if constexpr (std::is_same_v<T, std::nullptr_t>)
        {
            ctx.backend_->write_null();
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            ctx.backend_->write_bool(obj);
        }
        else
        {
            ctx.backend_->write_number(obj);
        }
    }

    template <typename T>
    requires std::is_fundamental_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, serializer_context& ctx)
    {
        if constexpr (std::is_same_v<T, std::nullptr_t>)
        {
            ctx.backend_->read_skip();
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            ctx.backend_->read_bool(obj);
        }
        else
        {
            ctx.backend_->read_number(obj);
        }
    }

    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void serialize(const T& obj, serializer_context& ctx)
    {
        using TEnum = std::remove_cvref_t<T>;

        if constexpr (format_enum_as_string_v<SerializerSettings>)
        {
            ctx.backend_->write_string(util::enum_to_string(obj));
        }
        else
        {
            ctx.backend_->write_number(static_cast<std::underlying_type_t<TEnum>>(obj));
        }
    }
        
    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, serializer_context& ctx)
    {
        using TEnum = std::remove_cvref_t<T>;
        using TIntegral = std::underlying_type_t<TEnum>;

        if constexpr (format_enum_as_string_v<SerializerSettings>)
        {
            obj = util::string_to_enum<TEnum>(ctx.backend_->read_string());
        }
        else
        {
            TIntegral value {};
            ctx.backend_->read_number(value);
            obj = static_cast<TEnum>(value);
        }
    }

    template <typename T>
    requires util::is_non_serializable_category_type_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void serialize(const T& obj, serializer_context& ctx)
    {
        static_assert_category_type<std::remove_cvref_t<T>>();
    }

    template <typename T>
    requires util::is_non_serializable_category_type_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CONSTEXPR void deserialize(T& obj, serializer_context& ctx)
    {
        static_assert_category_type<std::remove_cvref_t<T>>();
    }
};

}

#endif

