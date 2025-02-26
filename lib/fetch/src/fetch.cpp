#include "fetch.h"

#include <cpr/cpr.h>

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

using namespace cpr;

namespace fetch {
std::expected<std::vector<std::string>, fetch_error> fetch_region_codes(
    const std::string& region) {
    Response response = Get(Url{"https://restcountries.com/v3.1/region/" + region});

    if (response.status_code != 200) {
        return std::unexpected(fetch_error::status_code_not_200);
    }

    nlohmann::json countries;

    try {
        countries = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(fetch_error::parse_error);
    }

    std::vector<std::string> country_codes;

    for (const auto& country : countries) {
        if (!country.contains("cca2")) {
            return std::unexpected(fetch_error::value_not_found);
        }

        country_codes.emplace_back(country["cca2"]);
    }

    return country_codes;
}

std::expected<std::vector<std::string>, fetch_error> fetch_neighboring_countries(
    const std::string& api_key, const std::string& iso_code, const std::string& format) {
    Response response =
        Get(Url{"https://api.geodatasource.com/v2/neighboring-countries"},
            Parameters{{"key", api_key}, {"country_code", iso_code}, {"format", format}});

    if (response.status_code != 200) {
        return std::unexpected(fetch_error::status_code_not_200);
    }

    nlohmann::json neighbours;

    try {
        neighbours = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(fetch_error::parse_error);
    }

    std::vector<std::string> neighbouring_countries;

    for (const auto& neighbour : neighbours) {
        if (!neighbour.contains("country_code")) {
            return std::unexpected(fetch_error::value_not_found);
        }

        if (!neighbour.contains("country_name")) {
            return std::unexpected(fetch_error::value_not_found);
        }

        neighbouring_countries.emplace_back(neighbour["country_code"].get<std::string>());
    }

    return neighbouring_countries;
}

std::expected<country, fetch_error> fetch_country(const std::string& api_key,
                                                  const std::string& iso_code) {
    Response response = Get(Url{"https://restcountries.com/v3.1/alpha/" + iso_code});

    if (response.status_code != 200) {
        return std::unexpected(fetch_error::status_code_not_200);
    }

    nlohmann::json parsed;

    try {
        parsed = nlohmann::json::parse(response.text);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(fetch_error::parse_error);
    }

    nlohmann::json country = parsed.is_array() ? parsed[0] : parsed;

    if (!country.contains("name")) {
        return std::unexpected(fetch_error::value_not_found);
    }

    nlohmann::json name = country["name"];

    if (!name.contains("common")) {
        return std::unexpected(fetch_error::value_not_found);
    }

    struct country c;

    c.name = name["common"].get<std::string>();

    if (!country.contains("capital")) {
        return std::unexpected(fetch_error::value_not_found);
    }

    nlohmann::json capital =
        country["capital"].is_array() ? country["capital"][0] : country["capital"];

    c.capital = capital.get<std::string>();

    c.iso_code = iso_code;

    auto neighbouring_countries_result =
        fetch_neighboring_countries(api_key, iso_code, "json");

    if (!neighbouring_countries_result) {
        return std::unexpected(neighbouring_countries_result.error());
    }

    c.neighboring_countries_iso = neighbouring_countries_result.value();

    return c;
}

std::expected<std::unordered_map<std::string, country>, fetch_error> fetch_countries(
    const std::string& api_key, const std::vector<std::string>& iso_codes) {
    std::unordered_map<std::string, country> countries;

    for (size_t i = 0; i < iso_codes.size(); i++) {
        auto country_result = fetch_country(api_key, iso_codes[i]);

        if (!country_result) {
            return std::unexpected(country_result.error());
        }

        auto country = country_result.value();

        countries[country.iso_code] = country;
    }

    return countries;
}

}  // namespace fetch
