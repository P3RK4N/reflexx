#include "citm_catalog.hpp"

#include <concepts>
#include <yyjson.h>
#include <cassert>


inline void deserialize(std::string& out, yyjson_val* val)
{
    out = yyjson_get_str(val);
}

inline void deserialize(std::string_view& out, yyjson_val* val)
{
    out = { yyjson_get_str(val), yyjson_get_len(val) };
}

template <std::integral T>
inline void deserialize(T& out, yyjson_val* val)
    requires std::is_integral_v<T>
{
    if constexpr (std::is_signed_v<T>)
        out = static_cast<T>(yyjson_get_sint(val));
    else
        out = static_cast<T>(yyjson_get_uint(val));
}

template <typename T>
inline void deserialize(std::vector<T>& out, yyjson_val* arr)
{
    out.clear();

    yyjson_arr_iter iter;
    yyjson_arr_iter_init(arr, &iter);

    yyjson_val* val;
    while ((val = yyjson_arr_iter_next(&iter)))
    {
        T& elem = out.emplace_back();
        deserialize(elem, val);
    }
}

template <typename T>
inline void deserialize(std::optional<T>& out, yyjson_val* val)
{
    if (!val || yyjson_is_null(val))
    {
        out.reset();
        return;
    }

    T& obj = out.emplace();
    deserialize(obj, val);
}


template <typename TString, typename T>
inline void deserialize(std::map<TString, T>& out, yyjson_val* obj)
{
    out.clear();

    yyjson_obj_iter iter;
    yyjson_obj_iter_init(obj, &iter);

    yyjson_val* key;
    while ((key = yyjson_obj_iter_next(&iter)))
    {
        yyjson_val* val = yyjson_obj_iter_get_val(key);

        TString k;
        deserialize(k, key);

        T t;
        deserialize(t, val);

        out.emplace(std::move(k), std::move(t));
    }
}

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

// price
inline void deserialize(price& p, yyjson_val* obj)
{
    deserialize(p.amount, yyjson_obj_get(obj, "amount"));
    deserialize(p.audienceSubCategoryId, yyjson_obj_get(obj, "audienceSubCategoryId"));
    deserialize(p.seatCategoryId, yyjson_obj_get(obj, "seatCategoryId"));
}

// area
inline void deserialize(area& a, yyjson_val* obj)
{
    deserialize(a.areaId, yyjson_obj_get(obj, "areaId"));
    deserialize(a.blockIds, yyjson_obj_get(obj, "blockIds"));
}

// seat_category
inline void deserialize(seat_category& sc, yyjson_val* obj)
{
    deserialize(sc.areas, yyjson_obj_get(obj, "areas"));
    deserialize(sc.seatCategoryId, yyjson_obj_get(obj, "seatCategoryId"));
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
citm_catalog<TString> deserialize_citm_catalog(std::string_view json)
{
    // Parse JSON
    yyjson_doc* doc = yyjson_read(json.data(), json.size(), 0);
    yyjson_val* root = yyjson_doc_get_root(doc);

    citm_catalog<TString> catalog;

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

    yyjson_doc_free(doc);
    return catalog;
}

template citm_catalog<std::string> deserialize_citm_catalog<std::string>(std::string_view json);
template citm_catalog<std::string_view> deserialize_citm_catalog<std::string_view>(std::string_view json);
