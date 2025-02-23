#ifndef NEIGHBOUR_PARSER_H
#define NEIGHBOUR_PARSER_H

#include <expected>
#include <string>
#include <vector>

#include "code_parser.h"

namespace neighbour_parser {

struct country {
    const std::string name;
    const code_parser::country_code code;
    std::vector<country> neighbours;

    country(std::string country_name, code_parser::country_code country_code)
        : name(std::move(country_name)), code(std::move(country_code)) {}
};

std::expected<country, std::string> parse(const std::string& country_name,
                                          const code_parser::country_code& country_code,
                                          const std::string& json);
}  // namespace neighbour_parser

#endif  // !NEIGHBOUR_PARSER_H
