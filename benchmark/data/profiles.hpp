#ifndef REFLEXX_SIMPLE_HPP
#define REFLEXX_SIMPLE_HPP

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

#endif