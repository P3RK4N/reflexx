#include "citm_catalog.hpp"
#include "reflexx/builtin/backends/yyjson_backend.hpp"
#include "yyjson.h"
#include <memory>

// event<TString>
template <typename TString>
inline void deserialize(event<TString>& e, yyjson_val* obj)
{
    deserialize(e.description, yyjson_obj_get(obj, "description"));
    deserialize(e.id, yyjson_obj_get(obj, "id"));
    deserialize(e.logo, yyjson_obj_get(obj, "logo"));
    deserialize(e.name, yyjson_obj_get(obj, "name"));
    deserialize(e.subTopicIds, yyjson_obj_get(obj, "subTopicIds"));
    deserialize(e.subjectCode, yyjson_obj_get(obj, "subjectCode"));
    deserialize(e.subtitle, yyjson_obj_get(obj, "subtitle"));
    deserialize(e.topicIds, yyjson_obj_get(obj, "topicIds"));
}

template <typename TString>
inline void deserialize(event<TString>& e, yyjson_backend& backend)
{
    backend.read_begin_object();

    backend.read_key("description");
    deserialize(e.description, backend);

    backend.read_key("id");
    deserialize(e.id, backend);

    backend.read_key("logo");
    deserialize(e.logo, backend);

    backend.read_key("name");
    deserialize(e.name, backend);

    backend.read_key("subTopicIds");
    deserialize(e.subTopicIds, backend);

    backend.read_key("subjectCode");
    deserialize(e.subjectCode, backend);

    backend.read_key("subtitle");
    deserialize(e.subtitle, backend);

    backend.read_key("topicIds");
    deserialize(e.topicIds, backend);

    backend.read_end_object();
}

// price
inline void deserialize(price& p, yyjson_val* obj)
{
    deserialize(p.amount, yyjson_obj_get(obj, "amount"));
    deserialize(p.audienceSubCategoryId, yyjson_obj_get(obj, "audienceSubCategoryId"));
    deserialize(p.seatCategoryId, yyjson_obj_get(obj, "seatCategoryId"));
}

inline void deserialize(price& p, yyjson_backend& backend)
{
    backend.read_begin_object();

    backend.read_key("amount");
    deserialize(p.amount, backend);

    backend.read_key("audienceSubCategoryId");
    deserialize(p.audienceSubCategoryId, backend);

    backend.read_key("seatCategoryId");
    deserialize(p.seatCategoryId, backend);

    backend.read_end_object();
}

// area
inline void deserialize(area& a, yyjson_val* obj)
{
    deserialize(a.areaId, yyjson_obj_get(obj, "areaId"));
    deserialize(a.blockIds, yyjson_obj_get(obj, "blockIds"));
}

inline void deserialize(area& a, yyjson_backend& backend)
{
    backend.read_begin_object();

    backend.read_key("areaId");
    deserialize(a.areaId, backend);

    backend.read_key("blockIds");
    deserialize(a.blockIds, backend);

    backend.read_end_object();
}

// seat_category
inline void deserialize(seat_category& sc, yyjson_val* obj)
{
    deserialize(sc.areas, yyjson_obj_get(obj, "areas"));
    deserialize(sc.seatCategoryId, yyjson_obj_get(obj, "seatCategoryId"));
}

inline void deserialize(seat_category& sc, yyjson_backend& backend)
{
    backend.read_begin_object();

    backend.read_key("areas");
    deserialize(sc.areas, backend);

    backend.read_key("seatCategoryId");
    deserialize(sc.seatCategoryId, backend);

    backend.read_end_object();
}

// performance<TString>
template <typename TString>
inline void deserialize(performance<TString>& p, yyjson_val* obj)
{
    deserialize(p.eventId, yyjson_obj_get(obj, "eventId"));
    deserialize(p.id, yyjson_obj_get(obj, "id"));
    deserialize(p.logo, yyjson_obj_get(obj, "logo"));
    deserialize(p.name, yyjson_obj_get(obj, "name"));
    deserialize(p.prices, yyjson_obj_get(obj, "prices"));
    deserialize(p.seatCategories, yyjson_obj_get(obj, "seatCategories"));
    deserialize(p.seatMapImage, yyjson_obj_get(obj, "seatMapImage"));
    deserialize(p.start, yyjson_obj_get(obj, "start"));
    deserialize(p.venueCode, yyjson_obj_get(obj, "venueCode"));
}

template <typename TString>
inline void deserialize(performance<TString>& p, yyjson_backend& backend)
{
    backend.read_begin_object();

    backend.read_key("eventId");
    deserialize(p.eventId, backend);

    backend.read_key("id");
    deserialize(p.id, backend);

    backend.read_key("logo");
    deserialize(p.logo, backend);

    backend.read_key("name");
    deserialize(p.name, backend);

    backend.read_key("prices");
    deserialize(p.prices, backend);

    backend.read_key("seatCategories");
    deserialize(p.seatCategories, backend);

    backend.read_key("seatMapImage");
    deserialize(p.seatMapImage, backend);

    backend.read_key("start");
    deserialize(p.start, backend);

    backend.read_key("venueCode");
    deserialize(p.venueCode, backend);

    backend.read_end_object();
}

template <typename TString>
std::pair<citm_catalog<TString>, yyjson_doc_ptr_t> deserialize_citm_catalog(std::string_view json)
{
    // Parse JSON
    auto result      = std::pair{ citm_catalog<TString>{}, yyjson_doc_ptr_t{ yyjson_read(json.data(), json.size(), 0) } };
    auto& catalog    = result.first;
    auto& doc        = result.second;
    yyjson_val* root = yyjson_doc_get_root(doc.get());

    deserialize(catalog.areaNames, yyjson_obj_get(root, "areaNames"));
    deserialize(catalog.audienceSubCategoryNames, yyjson_obj_get(root, "audienceSubCategoryNames"));
    deserialize(catalog.blockNames, yyjson_obj_get(root, "blockNames"));
    deserialize(catalog.events, yyjson_obj_get(root, "events"));
    deserialize(catalog.performances, yyjson_obj_get(root, "performances"));
    deserialize(catalog.seatCategoryNames, yyjson_obj_get(root, "seatCategoryNames"));
    deserialize(catalog.subTopicNames, yyjson_obj_get(root, "subTopicNames"));
    deserialize(catalog.subjectNames, yyjson_obj_get(root, "subjectNames"));
    deserialize(catalog.topicNames, yyjson_obj_get(root, "topicNames"));
    deserialize(catalog.topicSubTopics, yyjson_obj_get(root, "topicSubTopics"));
    deserialize(catalog.venueNames, yyjson_obj_get(root, "venueNames"));

    return result;
}

template <typename TString>
std::pair<citm_catalog<TString>, yyjson_backend> backend_deserialize_citm_catalog(std::string_view json)
{
    auto result = std::pair{ citm_catalog<TString>{}, yyjson_backend{ json } };
    auto& catalog = result.first;
    auto& backend = result.second;

    backend.read_begin_object();

    backend.read_key("areaNames");
    deserialize(catalog.areaNames, backend);

    backend.read_key("audienceSubCategoryNames");
    deserialize(catalog.audienceSubCategoryNames, backend);

    backend.read_key("blockNames");
    deserialize(catalog.blockNames, backend);

    backend.read_key("events");
    deserialize(catalog.events, backend);

    backend.read_key("performances");
    deserialize(catalog.performances, backend);

    backend.read_key("seatCategoryNames");
    deserialize(catalog.seatCategoryNames, backend);

    backend.read_key("subTopicNames");
    deserialize(catalog.subTopicNames, backend);

    backend.read_key("subjectNames");
    deserialize(catalog.subjectNames, backend);

    backend.read_key("topicNames");
    deserialize(catalog.topicNames, backend);

    backend.read_key("topicSubTopics");
    deserialize(catalog.topicSubTopics, backend);

    backend.read_key("venueNames");
    deserialize(catalog.venueNames, backend);

    backend.read_end_object();

    return result;
}

// -------------------- normal deserialize with yyjson_doc --------------------
template std::pair<citm_catalog<std::string>, yyjson_doc_ptr_t>      
deserialize_citm_catalog<std::string>(std::string_view json);

template std::pair<citm_catalog<std::string_view>, yyjson_doc_ptr_t> 
deserialize_citm_catalog<std::string_view>(std::string_view json);

// -------------------- backend-based deserialize --------------------
template std::pair<citm_catalog<std::string>, yyjson_backend>      
backend_deserialize_citm_catalog<std::string>(std::string_view json);

template std::pair<citm_catalog<std::string_view>, yyjson_backend> 
backend_deserialize_citm_catalog<std::string_view>(std::string_view json);