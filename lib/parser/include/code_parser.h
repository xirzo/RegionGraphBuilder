#ifndef CODE_PARSER_H
#define CODE_PARSER_H

#include <expected>
#include <string>
#include <vector>

namespace code_parser {

struct country_code {
    const std::string iso_3166_2;
};

std::expected<std::vector<country_code>, std::string> parse(const std::string& json);

}  // namespace code_parser

#endif  // !CODE_PARSER_H
