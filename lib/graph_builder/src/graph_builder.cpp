#include "graph_builder.h"

#include <format>
#include <unordered_map>

#include "fetch.h"

graph_builder::error graph_builder::build(std::string_view region,
                                          std::string& error_details) {
    auto region_codes_result = fetch::fetch_region_codes(std::string(region));

    if (!region_codes_result) {
        error_details =
            std::format("region codes error: {} (region: {})",
                        static_cast<int>(region_codes_result.error()), region);
        return error::region_codes_failed;
    }

    auto countries_result =
        fetch::fetch_countries(geo_data_api_key_, region_codes_result.value());

    if (!countries_result) {
        error_details = std::format("countries error: {} (region: {})",
                                    static_cast<int>(countries_result.error()), region);
        return error::countries_failed;
    }

    std::unordered_map<std::string, std::string> name_to_iso_code;

    for (const auto& [_, country] : countries_result.value()) {
        name_to_iso_code[country.name] = country.iso_code;
    }

    return {};
}
