
#include "json_reader.h"

#include <expected>
#include <fstream>
#include <sstream>

namespace json_reader {
std::expected<std::string, std::string> read_file(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        return std::unexpected("Failed to open file: " + filepath);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    if (file.fail()) {
        return std::unexpected("Error reading file: " + filepath);
    }

    return buffer.str();
}
}  // namespace json_reader
