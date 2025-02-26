#ifndef JSON_FILE_H
#define JSON_FILE_H

#include <expected>
#include <nlohmann/json.hpp>
#include <string_view>

#include "nlohmann/json_fwd.hpp"

namespace json_file {

enum class write_error {

};

enum class read_error {

};

std::expected<void, write_error> write(std::string_view filename, nlohmann::json j);

std::expected<nlohmann::json, write_error> read(std::string_view filename);

}  // namespace json_file

#endif  // !JSON_FILE_H
