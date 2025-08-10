#ifndef REFLEXX_RESULT_HOLDER_HPP
#define REFLEXX_RESULT_HOLDER_HPP

#include "reflexx/declare.hpp"
#include <type_traits>


namespace reflexx {
namespace detail {

class result_holder_base
{
    template <serializer_settings S, IsBackendType B, IsTypeHandlerList D>
    friend class ::reflexx::serializer;

protected:
    using ctx_deleter_t = void (*)(void*);

    template <typename TCtx>
    inline constexpr result_holder_base(TCtx* ctx, ctx_deleter_t ctx_deleter) noexcept
    : ctx_(static_cast<void*>(ctx)), ctx_deleter_(ctx_deleter) {}

    inline constexpr result_holder_base(result_holder_base&& other) noexcept
    : ctx_(other.ctx_), ctx_deleter_(other.ctx_deleter_)
    {
        other.ctx_ = nullptr;
        other.ctx_deleter_ = nullptr;
    }

    inline constexpr result_holder_base& operator=(result_holder_base&& other) noexcept
    {
        if (this != &other)
        {
            reset();

            ctx_ = other.ctx_;
            ctx_deleter_ = other.ctx_deleter_;

            other.ctx_ = nullptr;
            other.ctx_deleter_ = nullptr;
        }
        return *this;
    }
    
    result_holder_base(const result_holder_base&) = delete;
    result_holder_base& operator=(const result_holder_base&) = delete;

    inline constexpr ~result_holder_base() noexcept
    {
        reset();
    }

private:
    inline constexpr void reset() noexcept
    {
        if (ctx_ && ctx_deleter_)
        {
            ctx_deleter_(ctx_);
            ctx_ = nullptr;
            ctx_deleter_ = nullptr;
        }
    }

    void* ctx_;
    ctx_deleter_t ctx_deleter_;
};

template <typename T>
static inline constexpr void deleter(void* ctx) noexcept
{
    delete static_cast<T*>(ctx);
}

} // detail


template <typename T>
class result_holder final : public detail::result_holder_base
{
    template <serializer_settings S, IsBackendType B, IsTypeHandlerList D>
    friend class serializer;

    using detail::result_holder_base::ctx_deleter_t;

private:
    template <typename TCtx>
    inline constexpr result_holder(T&& result, TCtx* ctx, ctx_deleter_t ctx_deleter = detail::deleter<TCtx>)
    noexcept(std::is_nothrow_move_constructible_v<T>)
    : res_(std::move(result)), detail::result_holder_base(ctx, ctx_deleter) {}
    
public:
    inline constexpr result_holder(result_holder&& other)               = default;
    inline constexpr result_holder& operator=(result_holder&& other)    = default;
    inline constexpr ~result_holder()                                   = default;
    inline constexpr result_holder(const result_holder&)                = delete;
    inline constexpr result_holder& operator=(const result_holder&)     = delete;
    
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
    T res_;
};

template <>
class result_holder<void> final : public detail::result_holder_base
{
    template <serializer_settings S, IsBackendType B, IsTypeHandlerList D>
    friend class serializer;

private:
    template <typename TCtx>
    inline constexpr result_holder(TCtx* ctx, ctx_deleter_t ctx_deleter = detail::deleter<TCtx>) noexcept
    : detail::result_holder_base(ctx, ctx_deleter) {}
};


}

#endif