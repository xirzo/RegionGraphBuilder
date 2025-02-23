#ifndef NAME_PARSER_H
#define NAME_PARSER_H

#include <expected>
#include <string>

namespace name_parser {
std::expected<std::string, std::string> parse(const std::string& text);
}  // namespace name_parser

#endif  // !NAME_PARSER_H
