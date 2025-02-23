#ifndef JSON_READER_H
#define JSON_READER_H

#include <expected>
#include <string>

namespace json_reader {
std::expected<std::string, std::string> read_file(const std::string& filepath);
}

#endif  // !JSON_READER_H
