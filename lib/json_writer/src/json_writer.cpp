#include "json_writer.h"

#include <exception>
#include <expected>
#include <fstream>

#include "nlohmann/json.hpp"

namespace json_writer {
std::expected<void, std::string> write_to_file(const std::string& text,
                                               const std::string& filepath) {
    try {
        nlohmann::json parsed = nlohmann::json::parse(text);

        std::ofstream file(filepath);

        if (!file.is_open()) {
            return std::unexpected("Failed to open file: " + filepath);
        }

        file << std::setw(4) << parsed << std::endl;

        if (file.fail()) {
            return std::unexpected("Failed to write to file: " + filepath);
        }

        return {};
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(e.what());
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::string> write_json_to_file(const nlohmann::json& j,
                                                    const std::string& filepath) {
    std::ofstream file(filepath);

    if (!file.is_open()) {
        return std::unexpected("Failed to open file: " + filepath);
    }

    file << j.dump(4) << std::endl;

    if (file.fail()) {
        return std::unexpected("Failed to write to file: " + filepath);
    }

    return {};
}
}  // namespace json_writer
