#include "graph_builder.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "country.h"
#include "fetch.h"
#include "json_file.h"
#include "nlohmann/json_fwd.hpp"

std::expected<std::unordered_map<std::string, country>, graph_builder::error>
fetch_countries(std::string_view region, std::string& error_details,
                const std::string& geo_data_api_key) {
    auto region_codes_result = fetch::fetch_region_codes(std::string(region));

    if (!region_codes_result) {
        error_details =
            std::format("region codes fetch error: {} (region: {})",
                        static_cast<int>(region_codes_result.error()), region);
        return std::unexpected(graph_builder::error::region_codes_failed);
    }

    auto countries_result =
        fetch::fetch_countries(geo_data_api_key, region_codes_result.value());

    if (!countries_result) {
        error_details = std::format("countries fetch error: {} (region: {})",
                                    static_cast<int>(countries_result.error()), region);
        return std::unexpected(graph_builder::error::countries_failed);
    }

    return countries_result.value();
}

graph_builder::error graph_builder::build(const std::string& region,
                                          std::string& error_details) {
    std::unordered_map<std::string, country> countries;

    if (!std::filesystem::exists(region)) {
        auto countries_result = fetch_countries(region, error_details, geo_data_api_key_);

        if (!countries_result) {
            return countries_result.error();
        }

        countries = countries_result.value();

        nlohmann::json j_umap(countries);

        auto write_result = json_file::write(region, j_umap);

        if (!write_result) {
            error_details = std::format("countries write to file error: {}",
                                        static_cast<int>(write_result.error()));
            return error::countries_write_failed;
        }
    } else {
        auto region_result = json_file::read(region);

        if (!region_result) {
            return error::read_region_file_error;
        }

        countries = region_result.value();
    }

    std::unordered_map<std::string, std::string> name_to_iso_code;

    for (const auto& [_, country] : countries) {
        name_to_iso_code[country.name] = country.iso_code;
    }

    for (const auto& [_, country] : countries) {
        std::cout << country.name << std::endl;
    }

    return {};
}
