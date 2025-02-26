#include "json_file.h"

namespace json_file {

std::expected<void, write_error> write(std::string_view filename, nlohmann::json j);

std::expected<nlohmann::json, write_error> read(std::string_view filename);

}  // namespace json_file
