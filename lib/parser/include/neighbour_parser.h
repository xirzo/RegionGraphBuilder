#ifndef NEIGHBOUR_PARSER_H
#define NEIGHBOUR_PARSER_H

#include <expected>
#include <string>
#include <vector>

#include "code_parser.h"

namespace neighbour_parser {

struct country {
    const code_parser::country_code code;
    std::vector<code_parser::country_code> neighbours;

    country(code_parser::country_code country_code) : code(std::move(country_code)) {}
};

std::expected<country, std::string> parse(const code_parser::country_code& country_code,
                                          const nlohmann::json& neighbours);
}  // namespace neighbour_parser

#endif  // !NEIGHBOUR_PARSER_H
