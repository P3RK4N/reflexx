#include "profiles.hpp"

template <typename TString>
std::vector<profile<TString>> deserialize_profiles(std::string_view json)
{
    std::vector<profile<TString>> profiles;

    yyjson_doc* doc = yyjson_read(json.data(), json.size(), 0);
    yyjson_val* arr = yyjson_doc_get_root(doc);

    yyjson_arr_iter iter;
    yyjson_arr_iter_init(arr, &iter);
    yyjson_val* val = nullptr;
    while ((val = yyjson_arr_iter_next(&iter))) 
    {
        profile<TString> p;
        deserialize(p.name, yyjson_obj_get(val, "name"));
        deserialize(p.language, yyjson_obj_get(val, "language"));
        deserialize(p.id, yyjson_obj_get(val, "id"));
        deserialize(p.bio, yyjson_obj_get(val, "bio"));
        deserialize(p.version, yyjson_obj_get(val, "version"));
        profiles.emplace_back(std::move(p));
    }

    yyjson_doc_free(doc);
    return profiles;
}

// Explicit instantiations
template std::vector<profile<std::string>> deserialize_profiles<std::string>(std::string_view);
template std::vector<profile<std::string_view>> deserialize_profiles<std::string_view>(std::string_view);