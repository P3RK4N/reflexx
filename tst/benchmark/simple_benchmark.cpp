#include <numeric>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "reflexx/serializer.hpp"
#include "reflexx/backends/yyjson_backend.hpp"
#include "yyjson.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

using namespace std::chrono;

struct Area {
    uint64_t areaId;
    std::vector<uint64_t> blockIds;
};

struct SeatCategory {
    uint64_t seatCategoryId;
    std::vector<Area> areas;
};

struct Price {
    uint64_t amount;
    uint64_t audienceSubCategoryId;
    uint64_t seatCategoryId;
};

struct Performance {
    uint64_t eventId;
    uint64_t id;
    std::optional<std::string> logo;
    std::optional<std::string> name;
    std::vector<Price> prices;
    std::vector<SeatCategory> seatCategories;
    std::optional<std::string> seatMapImage;
    uint64_t start; // Unix timestamp millis
    std::string venueCode;
};

struct Event {
    uint64_t id;
    std::optional<std::string> description;
    std::optional<std::string> logo;
    std::string name;
    std::vector<uint64_t> subTopicIds;
    std::vector<uint64_t> topicIds;
    std::optional<std::string> subjectCode;
    std::optional<std::string> subtitle;
};

struct CitmCatalog {
    std::unordered_map<std::string, std::string> areaNames;
    std::unordered_map<std::string, std::string> audienceSubCategoryNames;
    std::unordered_map<std::string, std::string> blockNames;
    std::unordered_map<std::string, Event> events;
    std::vector<Performance> performances;
};


std::string read_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    auto size = ifs.tellg();
    ifs.seekg(0);
    std::string buffer(size, '\0');
    ifs.read(buffer.data(), size);
    return buffer;
}

bool parse_citm_catalog(const std::string& json_data, CitmCatalog& catalog) {
    yyjson_doc* doc = yyjson_read(json_data.data(), json_data.size(), YYJSON_READ_ALLOW_COMMENTS);

    if (!doc) return false;
    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!root || !yyjson_is_obj(root)) {
        yyjson_doc_free(doc);
        return false;
    }

    // Helper lambda to parse string map
    auto parse_str_map = [](yyjson_val* obj, std::unordered_map<std::string, std::string>& out) {
        if (!obj || !yyjson_is_obj(obj)) return;
        yyjson_obj_iter iter;
        yyjson_obj_iter_init(obj, &iter);
        yyjson_val *key, *val;

        while ((key = yyjson_obj_iter_next(&iter))) {
            val = yyjson_obj_iter_get_val(key);
            out.emplace(std::string(yyjson_get_str(key)), std::string(yyjson_get_str(val)));
        }
    };

    // Parse simple maps
    parse_str_map(yyjson_obj_get(root, "areaNames"), catalog.areaNames);
    parse_str_map(yyjson_obj_get(root, "audienceSubCategoryNames"), catalog.audienceSubCategoryNames);
    parse_str_map(yyjson_obj_get(root, "blockNames"), catalog.blockNames);

    // Parse Events (map<string, Event>)
    yyjson_val* events_val = yyjson_obj_get(root, "events");
    if (events_val && yyjson_is_obj(events_val)) {
        yyjson_obj_iter iter;
        yyjson_obj_iter_init(events_val, &iter);
        yyjson_val *event_key, *event_val;
        while ((event_key = yyjson_obj_iter_next(&iter))) {
            event_val = yyjson_obj_iter_get_val(event_key);
            Event ev{};
            auto id_val = yyjson_obj_get(event_val, "id");
            ev.id = (id_val && !yyjson_is_null(id_val)) ? yyjson_get_uint(id_val) : 0;
            ev.description = yyjson_obj_get(event_val, "description") && !yyjson_is_null(yyjson_obj_get(event_val, "description"))
                             ? std::optional<std::string>{yyjson_get_str(yyjson_obj_get(event_val, "description"))} : std::nullopt;
            ev.logo = yyjson_obj_get(event_val, "logo") && !yyjson_is_null(yyjson_obj_get(event_val, "logo"))
                      ? std::optional<std::string>{yyjson_get_str(yyjson_obj_get(event_val, "logo"))} : std::nullopt;
            ev.name = yyjson_get_str(yyjson_obj_get(event_val, "name"));

            // Parse subTopicIds (array of uint64_t)
            yyjson_val* subTopicIds_val = yyjson_obj_get(event_val, "subTopicIds");
            if (subTopicIds_val && yyjson_is_arr(subTopicIds_val)) {
                yyjson_arr_iter arr_iter;
                yyjson_val* val;
                yyjson_arr_iter_init(subTopicIds_val, &arr_iter);
                while ((val = yyjson_arr_iter_next(&arr_iter))) {
                    ev.subTopicIds.push_back(yyjson_get_uint(val));
                }
            }

            // Parse topicIds (array of uint64_t)
            yyjson_val* topicIds_val = yyjson_obj_get(event_val, "topicIds");
            if (topicIds_val && yyjson_is_arr(topicIds_val)) {
                yyjson_arr_iter topic_iter;
                yyjson_val* topic_val;
                yyjson_arr_iter_init(topicIds_val, &topic_iter);
                while ((topic_val = yyjson_arr_iter_next(&topic_iter))) {
                    ev.topicIds.push_back(yyjson_get_uint(topic_val));
                }
            }

            ev.subjectCode = yyjson_obj_get(event_val, "subjectCode") && !yyjson_is_null(yyjson_obj_get(event_val, "subjectCode"))
                             ? std::optional<std::string>{yyjson_get_str(yyjson_obj_get(event_val, "subjectCode"))} : std::nullopt;

            ev.subtitle = yyjson_obj_get(event_val, "subtitle") && !yyjson_is_null(yyjson_obj_get(event_val, "subtitle"))
                          ? std::optional<std::string>{yyjson_get_str(yyjson_obj_get(event_val, "subtitle"))} : std::nullopt;

            catalog.events.emplace(std::string(yyjson_get_str(event_key)), std::move(ev));
        }
    }

    // Parse performances (array)
    yyjson_val* perf_arr = yyjson_obj_get(root, "performances");

    auto parse_performance = [&](yyjson_val* perf_val) {
        Performance perf{};

        // Basic fields
        if (auto v = yyjson_obj_get(perf_val, "eventId")) perf.eventId = yyjson_get_uint(v);
        if (auto v = yyjson_obj_get(perf_val, "id")) perf.id = yyjson_get_uint(v);

        if (auto v = yyjson_obj_get(perf_val, "logo"); v && !yyjson_is_null(v))
            perf.logo = std::string(yyjson_get_str(v));

        if (auto v = yyjson_obj_get(perf_val, "name"); v && !yyjson_is_null(v))
            perf.name = std::string(yyjson_get_str(v));

        if (auto v = yyjson_obj_get(perf_val, "seatMapImage"); v && !yyjson_is_null(v))
            perf.seatMapImage = std::string(yyjson_get_str(v));

        if (auto v = yyjson_obj_get(perf_val, "start")) perf.start = yyjson_get_uint(v);

        if (auto v = yyjson_obj_get(perf_val, "venueCode"))
            perf.venueCode = std::string(yyjson_get_str(v));

        // Prices array
        if (auto prices_arr = yyjson_obj_get(perf_val, "prices"); prices_arr && yyjson_is_arr(prices_arr)) {
            yyjson_arr_iter price_iter;
            yyjson_val* price_val;
            yyjson_arr_iter_init(prices_arr, &price_iter);
            while ((price_val = yyjson_arr_iter_next(&price_iter))) {
                Price price{};
                if (auto v = yyjson_obj_get(price_val, "amount")) price.amount = yyjson_get_uint(v);
                if (auto v = yyjson_obj_get(price_val, "audienceSubCategoryId")) price.audienceSubCategoryId = yyjson_get_uint(v);
                if (auto v = yyjson_obj_get(price_val, "seatCategoryId")) price.seatCategoryId = yyjson_get_uint(v);
                perf.prices.push_back(std::move(price));
            }
        }

        // SeatCategories array
        if (auto sc_arr = yyjson_obj_get(perf_val, "seatCategories"); sc_arr && yyjson_is_arr(sc_arr)) {
            yyjson_arr_iter sc_iter;
            yyjson_val* sc_val;
            yyjson_arr_iter_init(sc_arr, &sc_iter);
            while ((sc_val = yyjson_arr_iter_next(&sc_iter))) {
                SeatCategory sc{};
                if (auto v = yyjson_obj_get(sc_val, "seatCategoryId")) sc.seatCategoryId = yyjson_get_uint(v);

                if (auto areas_arr = yyjson_obj_get(sc_val, "areas"); areas_arr && yyjson_is_arr(areas_arr)) {
                    yyjson_arr_iter area_iter;
                    yyjson_val* area_val;
                    yyjson_arr_iter_init(areas_arr, &area_iter);
                    while ((area_val = yyjson_arr_iter_next(&area_iter))) {
                        Area area{};
                        if (auto v = yyjson_obj_get(area_val, "areaId")) area.areaId = yyjson_get_uint(v);

                        if (auto blocks = yyjson_obj_get(area_val, "blockIds"); blocks && yyjson_is_arr(blocks)) {
                            yyjson_arr_iter block_iter;
                            yyjson_val* block_val;
                            yyjson_arr_iter_init(blocks, &block_iter);
                            while ((block_val = yyjson_arr_iter_next(&block_iter))) {
                                area.blockIds.push_back(yyjson_get_uint(block_val));
                            }
                        }
                        sc.areas.push_back(std::move(area));
                    }
                }

                perf.seatCategories.push_back(std::move(sc));
            }
        }

        return perf;
    };

    std::vector<Performance> performances;
    if (perf_arr && yyjson_is_arr(perf_arr)) {
        yyjson_arr_iter perf_iter;
        yyjson_val* perf_val;
        yyjson_arr_iter_init(perf_arr, &perf_iter);
        while ((perf_val = yyjson_arr_iter_next(&perf_iter))) {
            performances.push_back(parse_performance(perf_val));
        }
    }

    catalog.performances = std::move(performances);
    yyjson_doc_free(doc);

    return true;
}

template <typename Func>
void benchmark(const std::string& json_data, Func deserialize_func, const std::string& label, int iterations = 10) {
    std::vector<double> times_ms;
    times_ms.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto start = high_resolution_clock::now();

        deserialize_func(json_data);

        auto end = high_resolution_clock::now();
        auto dur = duration_cast<microseconds>(end - start);
        times_ms.push_back(dur.count() / 1000.0);
    }

    double sum = std::accumulate(times_ms.begin(), times_ms.end(), 0.0);
    double mean = sum / iterations;

    double sq_sum = std::inner_product(times_ms.begin(), times_ms.end(), times_ms.begin(), 0.0);
    double variance = sq_sum / iterations - mean * mean;
    double stddev = std::sqrt(variance);

    std::cout << "[" << label << "] Mean time: " << mean << " ms, Stddev: " << stddev << " ms\n";
}

struct User {
    std::string name;
    std::string language;
    std::string id;
    std::string bio;
    double version;
};

int main()
{
    int iterations = 100;

    std::string json_data = read_file("citm_catalog.json");
    std::string _5mb = read_file("5mb.json");

    benchmark(json_data,
        [](const std::string& data) {
            CitmCatalog result;
            parse_citm_catalog(data, result);
        },
        "yyjson",
        iterations
    );

    benchmark(_5mb,
        [](const std::string& data) {
            std::vector<User> users;

            yyjson_doc* doc = yyjson_read(data.c_str(), data.size(), 0);
            if (!doc) return;

            yyjson_val* root = yyjson_doc_get_root(doc);
            if (!yyjson_is_arr(root)) {
                yyjson_doc_free(doc);
                return;
            }

            yyjson_val* val;
            size_t idx, max;
            yyjson_arr_foreach(root, idx, max, val) {
                if (!yyjson_is_obj(val)) continue;

                User user;
                yyjson_val* v;

                if ((v = yyjson_obj_get(val, "name")))     user.name     = yyjson_get_str(v);
                if ((v = yyjson_obj_get(val, "language"))) user.language = yyjson_get_str(v);
                if ((v = yyjson_obj_get(val, "id")))       user.id       = yyjson_get_str(v);
                if ((v = yyjson_obj_get(val, "bio")))      user.bio      = yyjson_get_str(v);
                if ((v = yyjson_obj_get(val, "version")))  user.version  = yyjson_get_real(v);

                users.push_back(std::move(user));
            }

            yyjson_doc_free(doc);
        },
        "yyjson",
        iterations
    );    

    
    benchmark(json_data,
        [](const std::string& data) {
            using Serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), reflexx::backends::yyjson_backend>;
            CitmCatalog result;
            Serializer::deserialize(result, data);
        },
        "Reflexx",
        iterations
    );

    benchmark(_5mb,
        [](const std::string& data) {
            using Serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), reflexx::backends::yyjson_backend>;
            std::vector<User> result;
            Serializer::deserialize(result, data);
        },
        "Reflexx",
        iterations
    );

    return 0;
}