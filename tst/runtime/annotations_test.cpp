#include <catch2/catch_all.hpp>

#include <cstdio>
#include <reflexx/annotations.hpp>

using namespace reflexx::annotations;

struct [[=enable_annotations{}]] TestStruct {
    int a;

    [[=reflexx::annotations::rename{ "b_renamed" }]]
    float b;

    [[=ignore{}]]
    int c;
};

TEST_CASE("enable_annotations is detected", "[annotations]") {
    STATIC_REQUIRE(has_enabled_annotations(^^TestStruct));
}

TEST_CASE("ignore annotation is respected", "[annotations]") {
    STATIC_REQUIRE_FALSE(has_ignore_annotation(^^TestStruct::a));
    STATIC_REQUIRE_FALSE(has_ignore_annotation(^^TestStruct::b));
    STATIC_REQUIRE(has_ignore_annotation(^^TestStruct::c));
}

TEST_CASE("rename annotation is respected", "[annotations]") {
    STATIC_REQUIRE_FALSE(get_annotated_name(^^TestStruct::a).has_value());
    STATIC_REQUIRE(*get_annotated_name(^^TestStruct::b) == "b_renamed");
    STATIC_REQUIRE_FALSE(get_annotated_name(^^TestStruct::c).has_value());
}
