#ifndef NAME_PARSER_H
#define NAME_PARSER_H

#include <expected>
#include <string>

#include "nlohmann/json_fwd.hpp"

namespace parser {
std::expected<std::string, std::string> parse_name(const nlohmann::json& parsed);
}  // namespace parser

#endif  // !NAME_PARSER_H
