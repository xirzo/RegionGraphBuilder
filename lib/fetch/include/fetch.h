#ifndef FETCH_H
#define FETCH_H

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace fetch {

enum class fetch_error {
    status_code_not_200,
    value_not_found,
    parse_error,
};

struct country {
    std::string name;
    std::string iso_code;
    std::string capital;
    std::vector<std::string> neighboring_countries_iso;
};

std::expected<std::vector<std::string>, fetch_error> fetch_region_codes(
    const std::string& region);

std::expected<country, fetch_error> fetch_country(const std::string& api_key,
                                                  const std::string& iso_code);

std::expected<std::unordered_map<std::string, country>, fetch_error> fetch_countries(
    const std::string& api_key, const std::vector<std::string>& iso_codes);
}  // namespace fetch

#endif  // !FETCH_H
