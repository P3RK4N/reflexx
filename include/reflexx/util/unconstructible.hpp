#ifndef REFLEXX_UNCONSTRUCTIBLE_HPP
#define REFLEXX_UNCONSTRUCTIBLE_HPP

namespace reflexx::util {

struct unconstructible
{
    unconstructible()                                   = delete;
    unconstructible(unconstructible&& other)            = delete;
    unconstructible(const unconstructible& other)       = delete;
    unconstructible& operator=(const unconstructible&)  = delete;
    unconstructible& operator=(unconstructible&&)       = delete;
};

} // reflexx::util

#endif