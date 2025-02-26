#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <expected>
#include <string>
#include <unordered_map>

#include "country.h"

class graph_builder {
public:
    enum class error { region_codes_failed, countries_failed };

    graph_builder(const std::string geo_data_api_key)
        : geo_data_api_key_(std::move(geo_data_api_key)) {}

    error build(std::string_view region, std::string& error_details);

private:
    const std::string geo_data_api_key_;

    std::expected<std::unordered_map<std::string, country>, graph_builder::error>
    fetch_countries(std::string_view region, std::string& error_details);
};
#endif  // !GRAPH_BUILDER_H
