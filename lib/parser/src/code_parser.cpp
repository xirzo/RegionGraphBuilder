#include "code_parser.h"

#include <expected>
#include <nlohmann/json.hpp>
#include <vector>

using nlohmann::json;

namespace parser {

std::expected<std::vector<country_code>, std::string> parse_code(
    const nlohmann::json& countries) {
    std::vector<country_code> country_codes;

    for (const auto& country : countries) {
        if (!country.contains("cca2")) {
            return std::unexpected(
                "Error: provided json`s countries do not contain cca2 value");
        }

        country_codes.emplace_back(country["cca2"]);
    }

    return country_codes;
}

};  // namespace parser
