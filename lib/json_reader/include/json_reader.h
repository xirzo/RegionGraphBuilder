#ifndef JSON_READER_H
#define JSON_READER_H

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace json_reader {

enum class read_file_error {
    file_does_not_exist,
    failed_to_open,
    failed_to_read,
    failed_to_parse_json
};

std::expected<nlohmann::json, read_file_error> read_file(
    const std::string& filepath) noexcept;
}  // namespace json_reader

#endif  // !JSON_READER_H
