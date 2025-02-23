#include "neighbour_parser.h"

#include <string>
#include <vector>

#include "code_parser.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

using nlohmann::json;

namespace neighbour_parser {

std::expected<country, std::string> parse(const std::string& country_name,
                                          const code_parser::country_code& country_code,
                                          const std::string& json) {
    struct country country{country_name, country_code};

    nlohmann::json neighbours = json::parse(json);

    for (const auto& neighbour : neighbours) {
        if (!neighbour.contains("country_code")) {
            return std::unexpected(
                "Error: provided json`s countries do not contain country_code value");
        }

        if (!neighbour.contains("country_name")) {
            return std::unexpected(
                "Error: provided json`s countries do not contain country_name value");
        }

        struct country neighbour_country(
            neighbour["country_name"].get<std::string>(),
            code_parser::country_code(neighbour["country_code"].get<std::string>()));

        country.neighbours.emplace_back(neighbour_country);
    }

    return country;
}

}  // namespace neighbour_parser
