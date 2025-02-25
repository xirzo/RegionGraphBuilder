#include "graph_builder.h"

#include <iostream>

#include "fetch.h"

void graph_builder::build(const std::string& region) {
    auto region_codes_result = fetch::fetch_region_codes(region);

    if (!region_codes_result) {
        switch (region_codes_result.error()) {
            case fetch::fetch_error::status_code_not_200:
                std::cerr << "error: fetching region codes status code is not 200"
                          << std::endl;
                break;
            case fetch::fetch_error::value_not_found:
                std::cerr << "error: fetching region codes value not found" << std::endl;
                break;
            case fetch::fetch_error::parse_error:
                std::cerr << "error: while parsing region codes" << std::endl;
                break;
        }
        return;
    }

    auto region_codes = region_codes_result.value();

    auto country_result = fetch::fetch_country(geo_data_api_key_, region_codes[0]);

    if (!country_result) {
        switch (country_result.error()) {
            case fetch::fetch_error::status_code_not_200:
                std::cerr << "error: fetching country status code is not 200"
                          << std::endl;
                break;
            case fetch::fetch_error::value_not_found:
                std::cerr << "error: fetching country value not found" << std::endl;
                break;
            case fetch::fetch_error::parse_error:
                std::cerr << "error: while parsing country code " << region_codes[0]
                          << std::endl;
                break;
        }

        return;
    }

    auto country = country_result.value();

    std::cout << country.name << std::endl;
    std::cout << country.iso_code << std::endl;
    std::cout << country.capital << std::endl;

    std::cout << "Neighboring countries iso" << std::endl;

    for (const auto& iso : country.neighboring_countries_iso) {
        std::cout << iso << std::endl;
    }
}
