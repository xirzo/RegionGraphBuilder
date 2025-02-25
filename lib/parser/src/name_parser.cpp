#include "name_parser.h"

#include "nlohmann/json.hpp"

namespace parser {

std::expected<std::string, std::string> parse_name(const nlohmann::json& parsed) {
    nlohmann::json country = parsed.is_array() ? parsed[0] : parsed;

    if (!country.contains("name")) {
        return std::unexpected(
            "Error: provided json's country does not contain name object");
    }

    nlohmann::json name = country["name"];

    if (!name.contains("common")) {
        return std::unexpected(
            "Error: provided json's name object does not contain common value");
    }

    return name["common"].get<std::string>();
}

}  // namespace parser
