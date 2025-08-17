#ifndef REFLEXX_ANNOTATIONS_HPP
#define REFLEXX_ANNOTATIONS_HPP

#include <experimental/meta>

#include <optional>
#include <string_view>

namespace reflexx::annotations {

/*
    #########################################################################
    ########################### Annotations #################################    
    #########################################################################
*/

// Class annotation
struct enable_annotations;

// Member annotations, ordered by precedence
struct ignore;
struct rename;

/*
    #########################################################################
    ########################## Annotations impl #############################    
    #########################################################################
*/

struct enable_annotations
{
    bool operator==(const enable_annotations&) const = default;
};

struct rename
{
    using static_string_t = decltype(std::define_static_string(std::declval<std::string_view>()));

    consteval rename(std::string_view name) : name_(std::define_static_string(name)) {}
    static_string_t name_;

    bool operator==(const rename&) const = default;
};

struct ignore 
{
    bool operator==(const ignore&) const = default;
};


/*
    #########################################################################
    ########################## Annotations util #############################    
    #########################################################################
*/

consteval bool has_enabled_annotations(std::meta::info type_info)
{
    return std::meta::annotation_of_type<annotations::enable_annotations>(type_info).has_value();
}

consteval bool has_ignore_annotation(std::meta::info member_info)
{
    return std::meta::annotation_of_type<annotations::ignore>(member_info).has_value();
}

consteval std::optional<std::string_view> get_annotated_name(std::meta::info member_info)
{
    auto name = std::meta::annotation_of_type<annotations::rename>(member_info);
    return name.has_value() ? std::make_optional(name->name_) : std::nullopt;
}

} // reflexx::annotations

#endif