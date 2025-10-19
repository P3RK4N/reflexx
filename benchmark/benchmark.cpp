#include "benchmark.hpp"

#ifdef NDEBUG
#   define CITM_CATALOG_ITER 200
#   define PROFILES_ITER     100
#else
#   define CITM_CATALOG_ITER 1
#   define PROFILES_ITER     1
#endif

/*
    ###############################################################################
    #                                 BASELINES                                   #
    ###############################################################################
*/

BASELINE(JSON_DESERIALIZE_CITM_CATALOG, MANUAL_YYJSON, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(deserialize_citm_catalog<std::string>(CITM_CATALOG_JSON));
}

BASELINE(JSON_DESERIALIZE_PROFILES, MANUAL_YYJSON, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(deserialize_profiles<std::string>(PROFILES_JSON));
}

/*
    ###############################################################################
    #                                BENCHMARKS                                   #
    ###############################################################################
*/

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, MANUAL_YYJSON_ZERO_COPY, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(deserialize_citm_catalog<std::string_view>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, MANUAL_YYJSON_BACKEND, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(backend_deserialize_citm_catalog<std::string>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, MANUAL_YYJSON_BACKEND_ZERO_COPY, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(backend_deserialize_citm_catalog<std::string_view>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLECT_CPP, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(rfl::json::read<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, GLAZE, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(glz::read_json<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, GLAZE_ZERO_COPY, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(glz::read_json<citm_catalog<std::string_view>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLEXX_YYJSON, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(relaxed_serializer::deserialize<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLEXX_YYJSON_ZERO_COPY, 30, CITM_CATALOG_ITER)
{
    celero::DoNotOptimizeAway(relaxed_serializer::deserialize<citm_catalog<std::string_view>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, MANUAL_YYJSON_ZERO_COPY, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(deserialize_profiles<std::string_view>(PROFILES_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, REFLECT_CPP, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(rfl::json::read<std::vector<profile<std::string>>>(PROFILES_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, GLAZE, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(glz::read_json<std::vector<profile<std::string>>>(PROFILES_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, GLAZE_ZERO_COPY, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(glz::read_json<std::vector<profile<std::string_view>>>(PROFILES_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, REFLEXX_YYJSON, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(strict_serializer::deserialize<std::vector<profile<std::string>>>(PROFILES_JSON));
}

BENCHMARK(JSON_DESERIALIZE_PROFILES, REFLEXX_YYJSON_ZERO_COPY, 30, PROFILES_ITER)
{
    celero::DoNotOptimizeAway(strict_serializer::deserialize<std::vector<profile<std::string_view>>>(PROFILES_JSON));
}

CELERO_MAIN
