#ifndef REFLEXX_SERIALIZER_HPP
#define REFLEXX_SERIALIZER_HPP

#include <cstddef>
#include <memory>
#include <string_view>
#include <experimental/meta>
#include <type_traits>
#include <utility>
#include <vector>

#include "custom_type_serializer.hpp"
#include "concepts/checks.hpp"
#include "concepts/backend.hpp"
#include "serializer_settings.hpp"
#include "util/non_serializable_category_type.hpp"
#include "util/enum_conv.hpp"

namespace reflexx
{

using namespace ::reflexx::concepts;

// Crashes clangd if constexpr qualifier is included...
#define REFLEXX_INLINE_CX inline

template <serializer_settings SerializerSettings, IsBackendType BackendType>
class serializer final
{

/*
    #########################################################################
    ############################# Interface #################################    
    #########################################################################
*/
public:
    struct serializer_result final
    {
        friend class serializer;
    
    private:
        serializer_result() = default;
        std::unique_ptr<BackendType> backend_ = nullptr;

    public:
        inline constexpr std::string_view get() const
        {
            return backend_->get();
        }
    };

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX serializer_result serialize(const T& obj)
    {
        serializer_context ctx {};
        serializer_result result {};
        
        ctx.backend_ = std::make_unique<BackendType>();
        
        serialize(obj, ctx);
        
        result.backend_ = std::move(ctx.backend_);

        return result;
    };

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX void deserialize(T& obj, std::string_view text)
    {
        serializer_context ctx {};
        
        ctx.backend_ = std::make_unique<BackendType>(text);
        
        deserialize(obj, ctx);
    }

    template <typename T>
    requires std::is_class_v<T> && std::is_default_constructible_v<T> // TODO: || IsProvideable<T>
    static REFLEXX_INLINE_CX T deserialize(std::string_view text)
    {
        T t {};
        deserialize(t, text);
        return t;
    }

/*
    #########################################################################
    ############################## Internal #################################    
    #########################################################################
*/

friend class custom_type_serializer<serializer>;

private:
    template <typename T>
    static void assert_category_type()
    {
        static_assert
        (
            !is_non_serializable_category_type_v<T>,
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

    template <std::meta::info MemberInfo>
    requires (std::meta::is_nonstatic_data_member(MemberInfo))
    static void assert_member_type()
    {
        using TMember = typename[: std::meta::type_of(MemberInfo) :];

        static_assert
        (
            !std::is_const_v<TMember> && !std::is_reference_v<TMember>,
            "\n\n\n"
            "##########################################################\n"
            "################## ˇSERIALIZATION ERRORˇ #################\n"
            "##########################################################\n"
            "\n\n"
            "Nonstatic data members cannot be const or reference types!\n" 
            "Consider writing custom serializer for it.\n"
            "Note that deserialization into const types is undefined\n"
            "behaviour.\n"
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
    static REFLEXX_INLINE_CX void serialize(const T& obj, serializer_context& ctx)
    {
        using TObj = std::remove_cvref_t<T>;

        if constexpr (has_builtin_handler_v<TObj>)
        {
            serialize_builtin(obj, ctx);
        }
        else
        {
            serialize_default(obj, ctx);
        }
    }

    template <typename T>
    requires std::is_class_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX void deserialize(T& obj, serializer_context& ctx)
    {
        using TObj = std::remove_cvref_t<T>;

        if constexpr (has_builtin_handler_v<TObj>)
        {
            deserialize_builtin(obj, ctx);
        }
        else // Default fallback
        {
            deserialize_default(obj, ctx);
        }
    }

    template <typename T>
    requires std::is_bounded_array_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX void serialize(const T& obj, serializer_context& ctx)
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
    static REFLEXX_INLINE_CX void deserialize(T& obj, serializer_context& ctx)
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
    static REFLEXX_INLINE_CX void serialize(const T& obj, serializer_context& ctx)
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
    static REFLEXX_INLINE_CX void deserialize(T& obj, serializer_context& ctx)
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
    static REFLEXX_INLINE_CX void serialize(const T& obj, serializer_context& ctx)
    {
        using TEnum = std::remove_cvref_t<T>;

        if constexpr (format_enum_as_string_v<SerializerSettings>)
        {
            ctx.backend_->write_string(enum_to_string(obj));
        }
        else
        {
            ctx.backend_->write_number(static_cast<std::underlying_type_t<TEnum>>(obj));
        }
    }
        
    template <typename T>
    requires std::is_enum_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX void deserialize(T& obj, serializer_context& ctx)
    {
        using TEnum = std::remove_cvref_t<T>;
        using TIntegral = std::underlying_type_t<TEnum>;

        if constexpr (format_enum_as_string_v<SerializerSettings>)
        {
            obj = string_to_enum<TEnum>(ctx.backend_->read_string());
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
    static REFLEXX_INLINE_CX void serialize(const T& obj, serializer_context& ctx)
    {
        assert_category_type<std::remove_cvref_t<T>>();
    }

    template <typename T>
    requires util::is_non_serializable_category_type_v<std::remove_cvref_t<T>>
    static REFLEXX_INLINE_CX void deserialize(T& obj, serializer_context& ctx)
    {
        assert_category_type<std::remove_cvref_t<T>>();
    }

/*
    #########################################################################
    ######################## Default serializer #############################    
    #########################################################################
*/

    template <typename T>
    static REFLEXX_INLINE_CX void serialize_default(const T& obj, serializer_context& ctx)
    {
        static_assert(ValidationCheck<SerializerSettings, T>, "Checks not satisfied!");

        ctx.backend_->write_begin_object();

        serialize_slice(obj, ctx);

        ctx.backend_->write_end_object();
    }

    template <typename T>
    static REFLEXX_INLINE_CX void deserialize_default(T& obj, serializer_context& ctx)
    {
        static_assert(ValidationCheck<SerializerSettings, T>, "Checks not satisfied!");

        ctx.backend_->read_begin_object();
        
        deserialize_slice(obj, ctx);
        
        ctx.backend_->read_end_object();
    }

    template <typename T>
    static REFLEXX_INLINE_CX void serialize_slice(const T& obj, serializer_context& ctx)
    {
        const std::byte* obj_ptr = reinterpret_cast<const std::byte*>(&obj);

        template for (constexpr auto& base_info : std::define_static_array(std::meta::bases_of(^^T, std::meta::access_context::unchecked())))
        {
            using TBase = typename [: std::meta::type_of(base_info) :];

            constexpr std::ptrdiff_t slice_offset = std::meta::offset_of(base_info).bytes;
            const std::byte* base_ptr = obj_ptr + slice_offset;
            serialize_slice<TBase>(*reinterpret_cast<const TBase*>(base_ptr), ctx);
        }

        template for (constexpr auto& member_info : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())))
        {
            using TMember = [: std::meta::type_of(member_info) :];

            if constexpr (should_handle_member_v<SerializerSettings, member_info>)
            {
                assert_member_type<member_info>();
                ctx.backend_->write_key(std::meta::identifier_of(member_info));
                serialize(obj.[: member_info :], ctx);
            }
        }
    }

    template <typename T>
    static REFLEXX_INLINE_CX void deserialize_slice(T& obj, serializer_context& ctx)
    {
        std::byte* obj_ptr = reinterpret_cast<std::byte*>(&obj);

        template for (constexpr auto& base_info : std::define_static_array(std::meta::bases_of(^^T, std::meta::access_context::unchecked())))
        {
            using TBase = typename [: std::meta::type_of(base_info) :];

            constexpr std::ptrdiff_t slice_offset = std::meta::offset_of(base_info).bytes;
            std::byte* base_ptr = obj_ptr + slice_offset;
            deserialize_slice<TBase>(*reinterpret_cast<TBase*>(base_ptr), ctx);
        }

        template for (constexpr auto& member_info : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())))
        {
            using TMember = typename[: std::meta::type_of(member_info) :];

            if constexpr (should_handle_member_v<SerializerSettings, member_info>)
            {
                assert_member_type<member_info>();
                ctx.backend_->read_key(std::meta::identifier_of(member_info));
                deserialize(obj.[: member_info :], ctx);
            }
        }
    }

/*
    #########################################################################
    ######################### Built-in handlers #############################    
    #########################################################################
*/

    template <typename T>
    static REFLEXX_INLINE_CX void serialize_builtin(const T& obj, serializer_context& ctx)  = delete;

    template <typename T>
    static REFLEXX_INLINE_CX void deserialize_builtin(T& obj, serializer_context& ctx)      = delete;

    template <>
    REFLEXX_INLINE_CX void serialize_builtin(const std::string& obj, serializer_context& ctx)
    {
        ctx.backend_->write_string(obj);
    }

    template <>
    REFLEXX_INLINE_CX void deserialize_builtin(std::string& obj, serializer_context& ctx)
    {
        obj = ctx.backend_->read_string();
    }

    template <typename T, typename Alloc>
    static REFLEXX_INLINE_CX void serialize_builtin(const std::vector<T, Alloc>& obj, serializer_context& ctx)
    {
        // TODO: Add option to enable size inclusion, avoiding vector reallocs (or leave it to the user)
        ctx.backend_->write_begin_array();

            for (const auto& elem : obj)
            {
                serialize(elem, ctx);
            }

        ctx.backend_->write_end_array();
    }

    template <typename T, typename Alloc>
    static REFLEXX_INLINE_CX void deserialize_builtin(std::vector<T, Alloc>& vec, serializer_context& ctx)
    {
        vec.clear();

        ctx.backend_->read_begin_array();

            while (ctx.backend_->read_has_next())
            {
                vec.emplace_back();
                deserialize(vec.back(), ctx);
            }
                
        ctx.backend_->read_end_array();
    }

    template <typename T>
    static constexpr bool has_builtin_handler_v = requires (T obj, serializer_context& ctx)
    {
        serialize_builtin(obj, ctx);
        deserialize_builtin(obj, ctx);
    };
};

}

#endif

