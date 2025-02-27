#include "json_file.h"

#include <fstream>
#include <system_error>

#include "nlohmann/json_fwd.hpp"

namespace json_file {

std::expected<void, error_info> write(const std::string& filename,
                                      const nlohmann::json& j) {
    std::ofstream output(filename);

    if (!output.is_open()) {
        return std::unexpected(make_error(
            error_info::code::cannot_open_file,
            "Failed to open file '" + filename + "' for writing", "file_write", filename,
            "System error: " + std::generic_category().message(errno)));
    }

    try {
        output << j.dump(4);

        if (output.fail()) {
            return std::unexpected(
                make_error(error_info::code::cannot_open_file,
                           "Failed to write to file '" + filename + "'", "json_write",
                           filename, "Stream failure during write operation"));
        }
    } catch (const std::exception& e) {
        return std::unexpected(
            make_error(error_info::code::cannot_open_file,
                       "Exception while writing to file '" + filename + "'", "json_write",
                       filename, "Exception: " + std::string(e.what())));
    }

    return {};
}

std::expected<nlohmann::json, error_info> read(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return std::unexpected(make_error(
            error_info::code::cannot_open_file,
            "Failed to open file '" + filename + "' for reading", "file_read", filename,
            "System error: " + std::generic_category().message(errno)));
    }

    std::string content;
    try {
        content = std::string((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

        if (file.fail()) {
            return std::unexpected(
                make_error(error_info::code::cannot_open_file,
                           "Failed to read from file '" + filename + "'", "file_read",
                           filename, "Stream failure during read operation"));
        }
    } catch (const std::exception& e) {
        return std::unexpected(
            make_error(error_info::code::cannot_open_file,
                       "Exception while reading file '" + filename + "'", "file_read",
                       filename, "Exception: " + std::string(e.what())));
    }

    try {
        return nlohmann::json::parse(content);
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(make_error(
            error_info::code::failed_parsing,
            "Failed to parse JSON from file '" + filename + "'", "json_parse", filename,
            "Parse error at byte " + std::to_string(e.byte) + ": " + e.what()));
    }
}

}  // namespace json_file
