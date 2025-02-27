#ifndef FETCH_H
#define FETCH_H

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

#include "country.h"

namespace fetch {

struct error {
    enum class code {
        status_code_not_200,
        value_not_found,
        parse_error,
    };

    code error_code;
    std::string message;
    std::string context;
    int status_code{0};
    std::string raw_error{};
};

inline error make_error(error::code code, std::string message, std::string context = "",
                        int status_code = 0, std::string raw_error = "") {
    return error{.error_code = code,
                 .message = std::move(message),
                 .context = std::move(context),
                 .status_code = status_code,
                 .raw_error = std::move(raw_error)};
}

std::expected<std::vector<std::string>, error> fetch_region_codes(
    const std::string& region);

std::expected<country, error> fetch_country(const std::string& api_key,
                                            const std::string& iso_code);

std::expected<std::unordered_map<std::string, country>, error> fetch_countries(
    const std::string& api_key, const std::vector<std::string>& iso_codes);
}  // namespace fetch

#endif  // !FETCH_H
