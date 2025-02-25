#ifndef NAME_PARSER_H
#define NAME_PARSER_H

#include <expected>
#include <string>

#include "nlohmann/json_fwd.hpp"

namespace name_parser {
std::expected<std::string, std::string> parse(const nlohmann::json& parsed);
}  // namespace name_parser

#endif  // !NAME_PARSER_H
