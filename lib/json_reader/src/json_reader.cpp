
#include "json_reader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace json_reader {
std::expected<nlohmann::json, read_file_error> read_file(
    const std::string& filepath) noexcept {
    if (!std::filesystem::exists(filepath)) {
        return std::unexpected(read_file_error::file_does_not_exist);
    }

    std::ifstream file(filepath);

    if (!file.is_open()) {
        return std::unexpected(read_file_error::failed_to_open);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    if (file.fail()) {
        return std::unexpected(read_file_error::failed_to_read);
    }

    nlohmann::json j;

    try {
        j = nlohmann::json::parse(buffer.str());
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << e.what() << std::endl;
        return std::unexpected(read_file_error::failed_to_parse_json);
    }

    return j;
}
}  // namespace json_reader
