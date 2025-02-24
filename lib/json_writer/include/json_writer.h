#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <expected>
#include <string>

#include "nlohmann/json_fwd.hpp"

namespace json_writer {

std::expected<void, std::string> write_to_file(const std::string& text,
                                               const std::string& filepath);

std::expected<void, std::string> write_json_to_file(const nlohmann::json& j,
                                                    const std::string& filepath);
}  // namespace json_writer

#endif  // !JSON_WRITER_H
