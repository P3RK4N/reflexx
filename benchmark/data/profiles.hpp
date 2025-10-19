#ifndef REFLEXX_SIMPLE_HPP
#define REFLEXX_SIMPLE_HPP

#include "data_util.hpp"

static constexpr auto PROFILES_FILEPATH = PROJECT_ROOT "/benchmark/data/profiles.json";

template <typename TString>
struct profile
{
    TString name;
    TString language;
    TString id;  
    TString bio;
    float version;  
};

template <typename TString>
std::vector<profile<TString>> deserialize_profiles(std::string_view json);

#endif