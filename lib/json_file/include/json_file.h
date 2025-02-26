#ifndef JSON_FILE_H
#define JSON_FILE_H

#include <expected>
#include <nlohmann/json.hpp>

#include "nlohmann/json_fwd.hpp"

namespace json_file {

enum class write_error { cannot_open_file };

enum class read_error { cannot_open_file, failed_parsing };

std::expected<void, write_error> write(const std::string& filename, nlohmann::json j);

std::expected<nlohmann::json, read_error> read(const std::string& filename);

}  // namespace json_file

#endif  // !JSON_FILE_H
