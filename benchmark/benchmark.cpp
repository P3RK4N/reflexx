#include "benchmark.hpp"

BASELINE(JSON_DESERIALIZE_CITM_CATALOG, MANUAL, 30, 300)
{
    celero::DoNotOptimizeAway(deserialize_citm_catalog<std::string>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, MANUAL_ZERO_COPY, 30, 300)
{
    celero::DoNotOptimizeAway(deserialize_citm_catalog<std::string_view>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLECT_CPP, 30, 300)
{
    celero::DoNotOptimizeAway(rfl::json::read<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, GLAZE, 30, 300)
{
    celero::DoNotOptimizeAway(glz::read_json<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, GLAZE_ZERO_COPY, 30, 300)
{
    celero::DoNotOptimizeAway(glz::read_json<citm_catalog<std::string_view>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLEXX, 30, 300)
{
    celero::DoNotOptimizeAway(relaxed_serializer::deserialize<citm_catalog<std::string>>(CITM_CATALOG_JSON));
}

BENCHMARK(JSON_DESERIALIZE_CITM_CATALOG, REFLEXX_ZERO_COPY, 30, 300)
{
    celero::DoNotOptimizeAway(relaxed_serializer::deserialize<citm_catalog<std::string_view>>(CITM_CATALOG_JSON));
}

// BASELINE(JSON_DESERIALIZE_PROFILES, MANUAL, 30, 300)
// {
//     volatile uint64_t counter = 0;
//     while (counter < 1'000'000) counter++;
// }

// BENCHMARK(JSON_DESERIALIZE_PROFILES, REFLECT_CPP, 30, 300)
// {
//     celero::DoNotOptimizeAway(rfl::json::read<std::vector<profile<std::string>>>(CITM_CATALOG_JSON));
// }

CELERO_MAIN
