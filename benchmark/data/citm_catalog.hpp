#ifndef REFLEXX_CITM_CATALOG_HPP
#define REFLEXX_CITM_CATALOG_HPP

#include "data_util.hpp"

static constexpr auto CITM_CATALOG_FILEPATH = PROJECT_ROOT "/benchmark/data/citm_catalog.json";

template <typename TString>
struct event
{
    std::optional<TString> description;
    std::uint64_t id;
    std::optional<TString> logo;
    TString name;
    std::vector<std::uint64_t> subTopicIds;
    std::optional<TString> subjectCode;
    std::optional<TString> subtitle;
    std::vector<std::uint64_t> topicIds;
};

struct price
{
    std::uint64_t amount;
    std::uint64_t audienceSubCategoryId;
    std::uint64_t seatCategoryId;
};

struct area
{
    std::uint64_t areaId;
    std::vector<std::uint64_t> blockIds;
};

struct seat_category
{
    std::vector<area> areas;
    std::uint64_t seatCategoryId;
};

template <typename TString>
struct performance
{
    std::uint64_t eventId;
    std::uint64_t id;
    std::optional<TString> logo;
    std::optional<TString> name;
    std::vector<price> prices;
    std::vector<seat_category> seatCategories;
    std::optional<TString> seatMapImage;
    std::uint64_t start;
    TString venueCode;
};

template <typename TString = std::string>
struct citm_catalog
{
    using SSMap = std::map<TString, TString>;

    SSMap areaNames;
    SSMap audienceSubCategoryNames;
    SSMap blockNames;
    std::map<TString, event<TString>> events;
    std::vector<performance<TString>> performances;
    SSMap seatCategoryNames;
    SSMap subTopicNames;
    SSMap subjectNames;
    SSMap topicNames;
    std::map<TString, std::vector<std::uint64_t>> topicSubTopics;
    SSMap venueNames;
};

struct yyjson_doc_deleter
{
    void operator()(yyjson_doc* ptr) const noexcept
    {
        yyjson_doc_free(ptr);
    }
};

using yyjson_doc_ptr_t = std::unique_ptr<yyjson_doc, yyjson_doc_deleter>;

template <typename TString>
std::pair<citm_catalog<TString>, yyjson_doc_ptr_t> deserialize_citm_catalog(std::string_view json);

template <typename TString>
std::pair<citm_catalog<TString>, yyjson_backend> backend_deserialize_citm_catalog(std::string_view json);

#endif