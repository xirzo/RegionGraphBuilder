#ifndef FETCH_H
#define FETCH_H

#include <expected>
#include <string>

namespace fetch {
std::expected<std::string, std::string> fetch_region_codes(
    const std::string& region = "europe");

std::expected<std::string, std::string> fetch_country(const std::string& iso_code);

std::expected<std::string, std::string> fetch_neighboring_countries(
    const std::string& api_key, const std::string& country_iso3166_2_code,
    const std::string& format = "json");

}  // namespace fetch

#endif  // !FETCH_H
