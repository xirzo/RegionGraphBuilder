#include "neighbour_parser.h"

#include <string>

#include "nlohmann/json.hpp"

namespace parser {

std::expected<country, std::string> parse_neighbour(const country_code& country_code,
                                                    const nlohmann::json& neighbours) {
    struct country country{country_code};

    for (const auto& neighbour : neighbours) {
        if (!neighbour.contains("country_code")) {
            return std::unexpected(
                "Error: provided json`s countries do not contain country_code value");
        }

        if (!neighbour.contains("country_name")) {
            return std::unexpected(
                "Error: provided json`s countries do not contain country_name value");
        }

        country.neighbours.emplace_back(
            parser::country_code(neighbour["country_code"].get<std::string>()));
    }

    return country;
}

}  // namespace parser
