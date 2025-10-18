#ifndef REFLEXX_BENCHMARK_UTIL_HPP
#define REFLEXX_BENCHMARK_UTIL_HPP

#ifndef PROJECT_ROOT
    #error "Project root not defined"
#endif 

#include <cstdlib>
#include <fstream>
#include <cassert>

#include <celero/Celero.h>

#include <rfl/json.hpp>
#include <rfl.hpp>

#include <glaze/glaze.hpp>

#include <reflexx/serializer.hpp>
#include <reflexx/builtin/backends/yyjson_backend.hpp>

using namespace reflexx;
using relaxed_serializer = serializer<serializer_settings::Relaxed(), backends::yyjson_backend>;

inline std::string reflexx_load(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

#include "data/citm_catalog.hpp"
#include "data/profiles.hpp"

static const std::string CITM_CATALOG_JSON = []() { return reflexx_load(CITM_CATALOG_FILEPATH); }();
static const std::string PROFILES_JSON     = []() { return reflexx_load(PROFILES_FILEPATH);     }();

#endif