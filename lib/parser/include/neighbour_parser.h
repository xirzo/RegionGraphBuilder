#ifndef NEIGHBOUR_PARSER_H
#define NEIGHBOUR_PARSER_H

#include <expected>
#include <string>
#include <vector>

#include "code_parser.h"

namespace parser {

struct country {
    const country_code code;
    std::vector<country_code> neighbours;

    country(country_code country_code) : code(std::move(country_code)) {}
};

std::expected<country, std::string> parse_neighbour(const country_code& country_code,
                                                    const nlohmann::json& neighbours);
}  // namespace parser

#endif  // !NEIGHBOUR_PARSER_H
