#include "fetch.h"

#include <cpr/cpr.h>

#include <expected>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace cpr;

namespace fetch {

std::expected<std::vector<std::string>, error> fetch_region_codes(
    const std::string& region) {
    Response response = Get(Url{"https://restcountries.com/v3.1/region/" + region});

    if (response.status_code != 200) {
        return std::unexpected(
            make_error(error::code::status_code_not_200,
                       "Failed to fetch region codes for " + region,
                       "https://restcountries.com/v3.1/region/" + region,
                       response.status_code, response.text));
    }

    nlohmann::json countries;

    try {
        countries = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(
            make_error(error::code::parse_error, "Failed to parse JSON for " + region,
                       "JSON parsing" + region, response.status_code, e.what()));
    }

    std::vector<std::string> country_codes;

    for (const auto& country : countries) {
        if (!country.contains("cca2")) {
            return std::unexpected(make_error(
                error::code::value_not_found, "Country code (cca2) not found in response",
                "Field: cca2" + region, response.status_code, country.dump()));
        }

        country_codes.emplace_back(country["cca2"]);
    }

    return country_codes;
}

std::expected<std::vector<std::string>, error> fetch_neighboring_countries(
    const std::string& api_key, const std::string& iso_code, const std::string& format) {
    Response response =
        Get(Url{"https://api.geodatasource.com/v2/neighboring-countries"},
            Parameters{{"key", api_key}, {"country_code", iso_code}, {"format", format}});

    if (response.status_code != 200) {
        return std::unexpected(
            make_error(error::code::status_code_not_200,
                       "Failed to fetch neighboring countries for " + iso_code,
                       "https://api.geodatasource.com/v2/neighboring-countries",
                       response.status_code, response.text));
    }

    nlohmann::json neighbours;

    try {
        neighbours = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(
            make_error(error::code::parse_error,
                       "Failed to parse JSON neighboring countries for " + iso_code,
                       "JSON parsing", response.status_code, e.what()));
    }

    std::vector<std::string> neighbouring_countries;

    for (const auto& neighbour : neighbours) {
        if (!neighbour.contains("country_code")) {
            return std::unexpected(make_error(
                error::code::value_not_found, "Country code not found in response",
                "Field: country_code", response.status_code, neighbour.dump()));
        }

        if (!neighbour.contains("country_name")) {
            return std::unexpected(make_error(
                error::code::value_not_found, "Country name not found in response",
                "Field: country_name", response.status_code, neighbour.dump()));
        }

        neighbouring_countries.emplace_back(neighbour["country_code"].get<std::string>());
    }

    return neighbouring_countries;
}

std::expected<country, error> fetch_country(const std::string& api_key,
                                            const std::string& iso_code) {
    Response response = Get(Url{"https://restcountries.com/v3.1/alpha/" + iso_code});

    if (response.status_code != 200) {
        return std::unexpected(make_error(error::code::status_code_not_200,
                                          "Failed to fetch country data for " + iso_code,
                                          "GET /v3.1/alpha/" + iso_code,
                                          response.status_code, response.text));
    }

    nlohmann::json parsed;

    try {
        parsed = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(
            make_error(error::code::parse_error,
                       "Failed to parse JSON response for country " + iso_code,
                       "JSON parsing", response.status_code, e.what()));
    }

    nlohmann::json country = parsed.is_array() ? parsed[0] : parsed;

    if (!country.contains("name")) {
        return std::unexpected(make_error(
            error::code::value_not_found, "Name field not found in country data",
            "Field: name", response.status_code, country.dump()));
    }

    nlohmann::json name = country["name"];

    if (!name.contains("common")) {
        return std::unexpected(make_error(
            error::code::value_not_found, "Common field not found in country name data",
            "Field: common", response.status_code, name.dump()));
    }

    struct country c;

    c.name = name["common"].get<std::string>();

    if (!country.contains("capital")) {
        return std::unexpected(make_error(
            error::code::value_not_found, "Capital field not found in country data",
            "Field: capital", response.status_code, country.dump()));
    }

    nlohmann::json capital =
        country["capital"].is_array() ? country["capital"][0] : country["capital"];

    c.capital = capital.get<std::string>();

    if (!country.contains("capitalInfo")) {
        return std::unexpected(make_error(
            error::code::value_not_found, "Capital info field not found in country data",
            "Field: capitalInfo", response.status_code, country.dump()));
    }

    nlohmann::json capital_info = country["capitalInfo"];

    if (!capital_info.contains("latlng")) {
        return std::unexpected(
            make_error(error::code::value_not_found,
                       "Latitude and longitude field not found in capital info data",
                       "Field: latlng", response.status_code, capital_info.dump()));
    }

    nlohmann::json latitude_longitude = capital_info["latlng"];

    c.capital_coords = {.latitude = latitude_longitude[0],
                        .longitude = latitude_longitude[1]};

    c.iso_code = iso_code;

    auto neighbouring_countries_result =
        fetch_neighboring_countries(api_key, iso_code, "json");

    if (!neighbouring_countries_result) {
        return std::unexpected(neighbouring_countries_result.error());
    }

    c.neighboring_countries_iso = neighbouring_countries_result.value();

    return c;
}

std::expected<std::unordered_map<std::string, country>, error> fetch_countries(
    const std::string& api_key, const std::vector<std::string>& iso_codes) {
    std::unordered_map<std::string, country> countries;

    for (size_t i = 0; i < iso_codes.size(); i++) {
        auto country_result = fetch_country(api_key, iso_codes[i]);

        if (!country_result) {
            if (country_result.error().error_code == error::code::status_code_not_200) {
                std::cerr << "Fetch error: " << country_result.error().message
                          << "\nContext: " << country_result.error().context
                          << "\nStatus code: " << country_result.error().status_code
                          << "\nRaw error: " << country_result.error().raw_error
                          << std::endl;
                continue;
            }

            return std::unexpected(country_result.error());
        }

        auto country = country_result.value();
        countries[country.iso_code] = country;
    }

    return countries;
}

}  // namespace fetch
